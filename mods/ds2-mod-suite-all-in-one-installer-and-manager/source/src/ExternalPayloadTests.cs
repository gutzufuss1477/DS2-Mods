using System;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;

namespace DS2ModSuite
{
    internal static partial class SelfTest
    {
        private static void TestExternalPayloads(string testRoot, StringBuilder report)
        {
            Assert(!Assembly.GetExecutingAssembly().GetManifestResourceNames()
                .Any(name => name.StartsWith("DS2ModSuite.Payload.", StringComparison.Ordinal)),
                "mod binaries must be visible package files, not embedded resources");
            string root = Path.Combine(testRoot, "external-payload-tests");
            Directory.CreateDirectory(root);
            byte[] original = Encoding.UTF8.GetBytes("[Test]\r\nEnabled=1\r\n");
            string hash = HashUtil.BytesSha256(original);
            string file = Path.Combine(root, "test.ini");
            File.WriteAllBytes(file, original);
            using (Stream verified = EmbeddedData.OpenVerifiedPayload(root, "test.ini", hash.ToLowerInvariant()))
            {
                Assert(!verified.CanWrite, "verified payload stream must be read-only");
                File.WriteAllText(file, "tampered after verification");
                Assert(HashUtil.EqualsHash(HashUtil.StreamSha256(verified), hash),
                    "disk changes altered a verified payload stream");
            }
            ExpectPayloadFailure<InvalidDataException>(() => EmbeddedData.OpenVerifiedPayload(root, "test.ini", hash), "tampered file");
            ExpectPayloadFailure<FileNotFoundException>(() => EmbeddedData.OpenVerifiedPayload(root, "missing.ini", hash), "missing file");
            ExpectPayloadFailure<InvalidDataException>(() => EmbeddedData.OpenVerifiedPayload(root, "test.ini", "invalid"), "invalid hash");
            ExpectPayloadFailure<InvalidDataException>(() => EmbeddedData.OpenPayload(new string('0', 64)), "unlisted hash");
            ExpectPayloadFailure<InvalidOperationException>(() => EmbeddedData.OpenVerifiedPayload(root, "../test.ini", hash), "parent traversal");
            ExpectPayloadFailure<InvalidOperationException>(() => EmbeddedData.OpenVerifiedPayload(root, file, hash), "absolute path");
            ExpectPayloadFailure<InvalidOperationException>(() => EmbeddedData.OpenVerifiedPayload(root, "test.ini:stream", hash), "alternate data stream");
            using (var oversized = new FileStream(file, FileMode.Create, FileAccess.Write))
                oversized.SetLength(64L * 1024L * 1024L + 1L);
            ExpectPayloadFailure<InvalidDataException>(() => EmbeddedData.OpenVerifiedPayload(root, "test.ini", hash), "oversized file");
            File.Delete(file);
            report.AppendLine("PASS external payload integrity, immutable verified streams, missing/tampered files, hash allowlist, size and path guards; no embedded mod binaries");
        }

        private static void ExpectPayloadFailure<T>(Func<Stream> open, string scenario) where T : Exception
        {
            try { using (Stream stream = open()) { } }
            catch (T) { return; }
            throw new InvalidOperationException("Unsafe payload accepted: " + scenario);
        }
    }
}
