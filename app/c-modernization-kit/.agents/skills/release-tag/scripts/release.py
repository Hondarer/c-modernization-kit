#!/usr/bin/env python3
"""Plan releases with GET requests; publish only an explicitly approved plan."""

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path
from urllib.parse import quote


REPOSITORIES = (
    "c-modernization-kit", "make-framework", "googletest-c-framework",
    "doxygen-framework", "pub_markdown", "app_c-platform", "app_cjson",
    "app_porter", "app_sqlite", "app_lua", "app_zlib", "devbin-win",
    "oracle-linux-container",
)


class ReleaseError(RuntimeError):
    pass


class GitHub:
    def get(self, endpoint, optional=False):
        result = subprocess.run(
            ["gh", "api", "--method", "GET", "--include", endpoint],
            capture_output=True, text=True, encoding="utf-8",
        )
        output = result.stdout.replace("\r\n", "\n")
        header, separator, body = output.partition("\n\n")
        status = re.search(r"^HTTP/\S+ (\d{3})", header)
        code = int(status[1]) if status else None
        if optional and code == 404:
            return None
        if result.returncode or code != 200 or not separator:
            raise ReleaseError(f"GET {endpoint} failed (HTTP {code}): {result.stderr.strip()}")
        try:
            value = json.loads(body)
        except json.JSONDecodeError as error:
            raise ReleaseError(f"GET {endpoint}: invalid JSON") from error
        if not isinstance(value, dict):
            raise ReleaseError(f"GET {endpoint}: expected an object")
        return value

    def create_tag(self, repo, tag, sha):
        # Creating the ref first fails on a concurrent tag instead of adopting it.
        # see: https://docs.github.com/en/rest/git/refs#create-a-reference
        payload = json.dumps({"ref": f"refs/tags/{tag}", "sha": sha})
        result = subprocess.run(
            ["gh", "api", "--method", "POST", f"repos/{repo}/git/refs", "--input", "-"],
            input=payload, capture_output=True, text=True, encoding="utf-8",
        )
        if result.returncode:
            raise ReleaseError(f"Tag creation failed or outcome is unknown for {repo}: {result.stderr.strip()}")

    def create_release(self, repo, tag):
        result = subprocess.run(
            ["gh", "release", "create", tag, "--repo", repo,
             "--verify-tag", "--title", tag, "--generate-notes"],
            capture_output=True, text=True, encoding="utf-8",
        )
        if result.returncode:
            raise ReleaseError(f"Release creation failed or outcome is unknown for {repo}: {result.stderr.strip()}")


def checked_sha(value):
    if not isinstance(value, str) or not re.fullmatch(r"[0-9a-f]{40}", value):
        raise ReleaseError("A full commit SHA could not be resolved")
    return value


def plan(github, tag):
    if not re.fullmatch(r"v\d{8}\.\d+\.\d+", tag):
        raise ReleaseError("Tag must have the form vYYYYMMDD.major.minor")
    entries = []
    for name in REPOSITORIES:
        repo = f"Hondarer/{name}"
        base = f"repos/{repo}"
        metadata = github.get(base)
        branch = metadata.get("default_branch")
        if not isinstance(branch, str) or not branch:
            raise ReleaseError(f"Default branch unavailable: {repo}")
        head = github.get(f"{base}/commits/{quote(branch, safe='')}")
        sha = checked_sha(head.get("sha"))
        if github.get(f"{base}/git/ref/tags/{tag}", optional=True) is not None:
            raise ReleaseError(f"Existing tag: {repo} {tag}; no releases have been created by this check")
        latest = github.get(f"{base}/releases/latest", optional=True)
        latest_tag = latest.get("tag_name") if latest is not None else None
        latest_sha = None
        latest_url = None
        if latest is not None:
            if not isinstance(latest_tag, str) or not latest_tag:
                raise ReleaseError(f"Latest release has no tag: {repo}")
            commit = github.get(f"{base}/commits/{quote(latest_tag, safe='')}")
            latest_sha = checked_sha(commit.get("sha"))
            latest_url = latest.get("html_url")
        entries.append({"repo": repo, "branch": branch, "sha": sha,
                        "latest_tag": latest_tag, "latest_sha": latest_sha,
                        "latest_url": latest_url,
                        "action": "skip" if latest_sha == sha else "create"})
    return {"version": 1, "tag": tag, "entries": entries}


def apply(github, approved, emit=print):
    if not isinstance(approved, dict) or approved.get("version") != 1:
        raise ReleaseError("Unsupported plan")
    # Recheck every repository before the first write, including skip decisions.
    current = plan(github, approved.get("tag", ""))
    if current != approved:
        raise ReleaseError("Remote state or plan changed; review a new plan before publishing")
    tag = approved["tag"]
    completed = []
    for entry in approved["entries"]:
        repo = entry["repo"]
        if entry["action"] == "skip":
            emit(json.dumps({"repo": repo, "result": "skip", "url": entry["latest_url"]}))
            continue
        try:
            github.create_tag(repo, tag, entry["sha"])
            emit(json.dumps({"repo": repo, "result": "tag-created", "sha": entry["sha"]}))
            ref = github.get(f"repos/{repo}/git/ref/tags/{tag}")
            if ref.get("object", {}).get("sha") != entry["sha"]:
                raise ReleaseError(f"Tag SHA mismatch: {repo}")
            github.create_release(repo, tag)
            release = github.get(f"repos/{repo}/releases/tags/{tag}")
            commit = github.get(f"repos/{repo}/commits/{tag}")
            if (checked_sha(commit.get("sha")) != entry["sha"]
                    or release.get("tag_name") != tag or release.get("draft") is not False
                    or not release.get("html_url")):
                raise ReleaseError(f"Published release verification failed: {repo}")
            completed.append(repo)
            emit(json.dumps({"repo": repo, "result": "created", "url": release["html_url"]}))
        except (ReleaseError, OSError) as error:
            raise ReleaseError(
                f"{error}. Stopped at {repo}; completed: {completed}. "
                "Inspect this repository's tag and release before retrying. "
                "No automatic retry or rollback was performed."
            ) from error


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument("--plan", metavar="TAG", help="GET only; print a reviewable JSON plan")
    mode.add_argument("--apply", type=Path, metavar="APPROVED_PLAN", help="publish an explicitly approved JSON plan")
    args = parser.parse_args()
    github = GitHub()
    try:
        if args.plan:
            print(json.dumps(plan(github, args.plan), indent=2))
        else:
            apply(github, json.loads(args.apply.read_text(encoding="utf-8-sig")))
    except (ReleaseError, OSError, ValueError, TypeError, KeyError) as error:
        print(f"ERROR: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
