using System;
using System.IO;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;

class RedirectClangCl {
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    struct STARTUPINFO {
        public int cb;
        public string lpReserved;
        public string lpDesktop;
        public string lpTitle;
        public int dwX;
        public int dwY;
        public int dwXSize;
        public int dwYSize;
        public int dwXCountChars;
        public int dwYCountChars;
        public int dwFillAttribute;
        public int dwFlags;
        public short wShowWindow;
        public short cbReserved2;
        public IntPtr lpReserved2;
        public IntPtr hStdInput;
        public IntPtr hStdOutput;
        public IntPtr hStdError;
    }

    [StructLayout(LayoutKind.Sequential)]
    struct PROCESS_INFORMATION {
        public IntPtr hProcess;
        public IntPtr hThread;
        public int dwProcessId;
        public int dwThreadId;
    }

    [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
    static extern bool CreateProcessW(
        string lpApplicationName,
        string lpCommandLine,
        IntPtr lpProcessAttributes,
        IntPtr lpThreadAttributes,
        bool bInheritHandles,
        uint dwCreationFlags,
        IntPtr lpEnvironment,
        string lpCurrentDirectory,
        ref STARTUPINFO lpStartupInfo,
        out PROCESS_INFORMATION lpProcessInformation);

    [DllImport("kernel32.dll", SetLastError = true)]
    static extern uint WaitForSingleObject(IntPtr hHandle, uint dwMilliseconds);

    [DllImport("kernel32.dll", SetLastError = true)]
    static extern bool GetExitCodeProcess(IntPtr hProcess, out uint lpExitCode);

    [DllImport("kernel32.dll", SetLastError = true)]
    static extern bool CloseHandle(IntPtr hObject);

    static string braveChromiumSrcDir = null;

    static string EscapeArg(string arg) {
        if (string.IsNullOrEmpty(arg)) return "\"\"";
        if (arg.IndexOfAny(new char[] { ' ', '\t', '\n', '\v', '\"' }) == -1) return arg;
        var sb = new StringBuilder();
        sb.Append('\"');
        for (int i = 0; i < arg.Length; i++) {
            int backslashes = 0;
            while (i < arg.Length && arg[i] == '\\') {
                backslashes++;
                i++;
            }
            if (i == arg.Length) {
                sb.Append('\\', backslashes * 2);
                break;
            } else if (arg[i] == '\"') {
                sb.Append('\\', backslashes * 2 + 1);
                sb.Append('\"');
            } else {
                sb.Append('\\', backslashes);
                sb.Append(arg[i]);
            }
        }
        sb.Append('\"');
        return sb.ToString();
    }

    static string RedirectPath(string src) {
        string normalized = src.Replace('\\', '/');
        string relInsideSrc = null;

        if (normalized.StartsWith("../../")) {
            relInsideSrc = normalized.Substring(6);
        } else if (normalized.StartsWith("gen/")) {
            relInsideSrc = normalized;
        }

        if (relInsideSrc != null && !string.IsNullOrEmpty(braveChromiumSrcDir)) {
            string diskFile = Path.Combine(braveChromiumSrcDir, relInsideSrc.Replace('/', Path.DirectorySeparatorChar));
            if (File.Exists(diskFile)) {
                return "../../brave/chromium_src/" + relInsideSrc;
            }
        }
        return src;
    }

    static int Main(string[] args) {
        string exeDir = AppDomain.CurrentDomain.BaseDirectory;
        string realClang = Path.Combine(exeDir, "clang-cl-real.exe");

        // Walk up from clang directory to find brave/chromium_src
        try {
            DirectoryInfo cur = new DirectoryInfo(exeDir);
            while (cur != null) {
                string candidate = Path.Combine(cur.FullName, "brave", "chromium_src");
                if (Directory.Exists(candidate)) {
                    braveChromiumSrcDir = candidate;
                    break;
                }
                cur = cur.Parent;
            }
        } catch { }

        var sb = new StringBuilder();
        sb.Append(EscapeArg(realClang));

        for (int i = 0; i < args.Length; i++) {
            string arg = args[i];
            sb.Append(' ');
            if ((arg == "/c" || arg == "-c") && (i + 1 < args.Length)) {
                sb.Append(arg);
                sb.Append(' ');
                string src = args[++i];
                sb.Append(EscapeArg(RedirectPath(src)));
            } else if ((arg.StartsWith("/c", StringComparison.OrdinalIgnoreCase) || arg.StartsWith("-c", StringComparison.OrdinalIgnoreCase)) && arg.Length > 2) {
                string prefix = arg.Substring(0, 2);
                string src = arg.Substring(2);
                sb.Append(EscapeArg(prefix + RedirectPath(src)));
            } else {
                sb.Append(EscapeArg(arg));
            }
        }

        STARTUPINFO si = new STARTUPINFO();
        si.cb = Marshal.SizeOf(typeof(STARTUPINFO));
        PROCESS_INFORMATION pi;

        if (!CreateProcessW(null, sb.ToString(), IntPtr.Zero, IntPtr.Zero, true, 0, IntPtr.Zero, null, ref si, out pi)) {
            Console.Error.WriteLine("[redirect_cc error] Failed to execute clang-cl-real.exe");
            return 1;
        }

        WaitForSingleObject(pi.hProcess, 0xFFFFFFFF);
        uint exitCode = 0;
        GetExitCodeProcess(pi.hProcess, out exitCode);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return (int)exitCode;
    }
}
