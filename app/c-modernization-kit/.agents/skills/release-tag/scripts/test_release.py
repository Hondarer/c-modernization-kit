"""Release workflow tests using an in-memory remote; no network or Git writes."""

import base64
import json
import subprocess
import unittest
from unittest.mock import patch

import release


TAG = "v20260905.0.0"
SHA = "a" * 40
OLD = "b" * 40
PIN = "c" * 40
GITMODULES = """[submodule "lib/two"]
\tpath = lib/two
\turl = https://github.com/Hondarer/two.git
[submodule "lib/external"]
\tpath = lib/external
\turl = https://github.com/someone/external.git
"""


class Remote:
    def __init__(self):
        self.heads = {"one": SHA, "two": SHA}
        self.latest = {"one": OLD, "two": SHA}
        self.latest_tag = {"one": "previous", "two": "previous"}
        self.tags = {}
        self.published = set()
        self.writes = []
        self.failure = None
        self.fail_publish = None
        self.tag_override = None
        # Commits on each default branch; one@OLD pins two@PIN.
        self.branch_commits = {"one": {SHA, OLD}, "two": {SHA, PIN}, "three": {SHA}}

    def get(self, endpoint, optional=False):
        if endpoint == self.failure:
            raise release.ReleaseError("HTTP 503")
        name = endpoint.split("/")[2]
        suffix = "/".join(endpoint.split("/")[3:])
        if not suffix:
            return {"default_branch": "main"}
        if suffix == "commits/main":
            return {"sha": self.heads[name]}
        if suffix == "commits/bbbbbbb":
            return {"sha": OLD}
        if suffix == f"commits/{OLD}" and name == "one":
            return {"sha": OLD}
        if suffix == f"contents/.gitmodules?ref={OLD}" and name == "one":
            return {"content": base64.b64encode(GITMODULES.encode()).decode()}
        if suffix == f"contents/lib/two?ref={OLD}" and name == "one":
            return {"type": "submodule", "sha": PIN}
        if suffix.startswith("compare/"):
            sha = suffix.split("/")[1].split("...")[0]
            return {"status": "ahead" if sha in self.branch_commits[name] else "diverged"}
        if suffix == "releases/latest":
            return (None if self.latest[name] is None else
                    {"tag_name": self.latest_tag[name],
                     "html_url": f"https://example.test/{name}/{self.latest_tag[name]}"})
        if suffix == "commits/previous":
            return {"sha": self.latest[name]}
        if suffix == f"git/ref/tags/{TAG}":
            return {"object": {"sha": self.tags[name]}} if name in self.tags else None
        if suffix == f"commits/{TAG}":
            return {"sha": self.tags[name]}
        if suffix == f"releases/tags/{TAG}" and name in self.published:
            return {"tag_name": TAG, "draft": False, "html_url": f"https://example.test/{name}/{TAG}"}
        raise AssertionError(endpoint)

    def create_tag(self, repo, tag, sha):
        name = repo.split("/")[1]
        if name in self.tags:
            raise release.ReleaseError("Duplicate tag")
        self.writes.append(("tag", name, sha))
        self.tags[name] = self.tag_override or sha

    def create_release(self, repo, tag):
        name = repo.split("/")[1]
        self.writes.append(("release", name))
        if name == self.fail_publish:
            raise release.ReleaseError("Publish failed")
        self.published.add(name)


class WorkflowTests(unittest.TestCase):
    def setUp(self):
        self.names = patch.object(release, "REPOSITORIES", ("one", "two"))
        self.names.start()
        self.addCleanup(self.names.stop)
        self.remote = Remote()

    def test_plan_is_read_only_and_skips_matching_release(self):
        result = release.plan(self.remote, TAG)
        self.assertEqual([e["action"] for e in result["entries"]], ["create", "skip"])
        self.assertEqual(self.remote.writes, [])

    def test_missing_release_is_created(self):
        self.remote.latest["one"] = None
        approved = release.plan(self.remote, TAG)
        release.apply(self.remote, approved, lambda _: None)
        self.assertEqual(self.remote.published, {"one"})
        self.assertEqual(self.remote.tags, {"one": SHA})

    def test_duplicate_on_last_repository_prevents_all_writes(self):
        approved = release.plan(self.remote, TAG)
        self.remote.tags["two"] = OLD
        with self.assertRaises(release.ReleaseError):
            release.apply(self.remote, approved)
        self.assertEqual(self.remote.writes, [])

    def test_existing_tag_as_latest_release_at_head_is_skipped(self):
        self.remote.tags["two"] = SHA
        self.remote.latest_tag["two"] = TAG
        approved = release.plan(self.remote, TAG)
        self.assertEqual([e["action"] for e in approved["entries"]], ["create", "skip"])
        release.apply(self.remote, approved, lambda _: None)
        self.assertEqual(self.remote.writes, [("tag", "one", SHA), ("release", "one")])

    def test_existing_tag_as_latest_release_behind_head_is_rejected(self):
        self.remote.tags["two"] = OLD
        self.remote.latest_tag["two"] = TAG
        with self.assertRaisesRegex(release.ReleaseError, "Existing tag"):
            release.plan(self.remote, TAG)
        self.assertEqual(self.remote.writes, [])

    def test_failed_latest_lookup_prevents_all_writes(self):
        approved = release.plan(self.remote, TAG)
        self.remote.failure = "repos/Hondarer/two/releases/latest"
        with self.assertRaises(release.ReleaseError):
            release.apply(self.remote, approved)
        self.assertEqual(self.remote.writes, [])

    def test_unresolvable_latest_commit_is_not_treated_as_absent(self):
        self.remote.failure = "repos/Hondarer/one/commits/previous"
        with self.assertRaises(release.ReleaseError):
            release.plan(self.remote, TAG)
        self.assertEqual(self.remote.writes, [])

    def test_changed_head_requires_new_plan(self):
        approved = release.plan(self.remote, TAG)
        self.remote.heads["two"] = OLD
        with self.assertRaises(release.ReleaseError):
            release.apply(self.remote, approved)
        self.assertEqual(self.remote.writes, [])

    def test_modified_plan_is_rejected(self):
        approved = release.plan(self.remote, TAG)
        approved["entries"][0]["sha"] = OLD
        with self.assertRaises(release.ReleaseError):
            release.apply(self.remote, approved)
        self.assertEqual(self.remote.writes, [])

    def test_branch_movement_after_preflight_does_not_change_tag_sha(self):
        approved = release.plan(self.remote, TAG)
        original = self.remote.create_tag

        def move_then_create(repo, tag, sha):
            self.remote.heads["one"] = OLD
            original(repo, tag, sha)

        self.remote.create_tag = move_then_create
        release.apply(self.remote, approved, lambda _: None)
        self.assertEqual(self.remote.tags["one"], SHA)

    def test_publish_failure_stops_later_repositories_without_rollback(self):
        self.remote.latest["two"] = OLD
        approved = release.plan(self.remote, TAG)
        self.remote.fail_publish = "one"
        with self.assertRaisesRegex(release.ReleaseError, "Stopped at Hondarer/one"):
            release.apply(self.remote, approved, lambda _: None)
        self.assertEqual(self.remote.tags, {"one": SHA})
        self.assertEqual(self.remote.writes, [("tag", "one", SHA), ("release", "one")])

    def test_sha_mismatch_stops_before_release(self):
        approved = release.plan(self.remote, TAG)
        self.remote.tag_override = OLD
        with self.assertRaisesRegex(release.ReleaseError, "Tag SHA mismatch"):
            release.apply(self.remote, approved, lambda _: None)
        self.assertEqual(self.remote.published, set())


class WorkspacePinTests(unittest.TestCase):
    def setUp(self):
        for target, value in (("REPOSITORIES", ("one", "two", "three")), ("WORKSPACE", "one")):
            patcher = patch.object(release, target, value)
            patcher.start()
            self.addCleanup(patcher.stop)
        self.remote = Remote()
        self.remote.heads["three"] = SHA
        self.remote.latest["three"] = OLD
        self.remote.latest_tag["three"] = "previous"
        self.remote.latest["two"] = OLD

    def test_workspace_sha_pins_workspace_and_submodules(self):
        result = release.plan(self.remote, TAG, "bbbbbbb")
        self.assertEqual(result["workspace_sha"], OLD)
        self.assertEqual([(e["source"], e["sha"]) for e in result["entries"]],
                         [("workspace", OLD), ("workspace-submodule", PIN), ("default-branch", SHA)])
        self.assertEqual(self.remote.writes, [])

    def test_apply_tags_pinned_commits(self):
        self.remote.latest["one"] = None
        approved = release.plan(self.remote, TAG, "bbbbbbb")
        release.apply(self.remote, approved, lambda _: None)
        self.assertEqual(self.remote.tags, {"one": OLD, "two": PIN, "three": SHA})

    def test_pin_off_default_branch_is_rejected(self):
        self.remote.branch_commits["two"] = {SHA}
        with self.assertRaisesRegex(release.ReleaseError, "is not on main"):
            release.plan(self.remote, TAG, OLD)
        self.assertEqual(self.remote.writes, [])

    def test_invalid_workspace_sha_is_rejected(self):
        for value in ("main", "ABCDEF0", "abc"):
            with self.subTest(value=value), self.assertRaises(release.ReleaseError):
                release.plan(self.remote, TAG, value)


class TransportTests(unittest.TestCase):
    @patch.object(release.subprocess, "run")
    def test_optional_lookup_distinguishes_http_failures(self, run):
        for status in (401, 403, 429, 500, 503):
            with self.subTest(status=status):
                run.return_value = subprocess.CompletedProcess([], 1, f"HTTP/2.0 {status} Error\n\n{{}}", "error")
                with self.assertRaises(release.ReleaseError):
                    release.GitHub().get("repos/test/test/releases/latest", optional=True)
        run.return_value = subprocess.CompletedProcess([], 1, "HTTP/2.0 404 Not Found\n\n{}", "not found")
        self.assertIsNone(release.GitHub().get("repos/test/test/releases/latest", optional=True))

    @patch.object(release.subprocess, "run")
    def test_invalid_response_and_network_failure_stop(self, run):
        for output in ("", "HTTP/2.0 200 OK\n\nnot json", "HTTP/2.0 200 OK\n\n[]"):
            run.return_value = subprocess.CompletedProcess([], 0, output, "")
            with self.assertRaises(release.ReleaseError):
                release.GitHub().get("repos/test/test")

    @patch.object(release.subprocess, "run")
    def test_valid_response_uses_get(self, run):
        run.return_value = subprocess.CompletedProcess([], 0, 'HTTP/2.0 200 OK\r\nContent-Type: application/json\r\n\r\n{"sha":"abc"}', "")
        self.assertEqual(release.GitHub().get("repos/test/test"), {"sha": "abc"})
        self.assertIn("GET", run.call_args.args[0])

    @patch.object(release.subprocess, "run")
    def test_tag_creation_passes_full_sha_as_json(self, run):
        run.return_value = subprocess.CompletedProcess([], 0, "{}", "")
        release.GitHub().create_tag("Hondarer/one", TAG, SHA)
        self.assertEqual(json.loads(run.call_args.kwargs["input"]), {"ref": f"refs/tags/{TAG}", "sha": SHA})


if __name__ == "__main__":
    unittest.main()
