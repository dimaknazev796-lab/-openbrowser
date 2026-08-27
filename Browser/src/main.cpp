#include <iostream>
#include <string>
#include "app/browser_app.h"

// Define a stub for wWinMain to show Windows architecture entry point
#if defined(_WIN32)
#include <windows.h>
#ifdef HAS_CEF
#include "include/cef_app.h"
#include "include/cef_sandbox_win.h"
#include "renderer/render_app.h"
#endif

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                      LPTSTR lpCmdLine, int nCmdShow) {
#ifdef HAS_CEF
    // Secure CEF Initialization for Windows
    // Enable High-DPI support on Windows 7 or newer.
    CefEnableHighDPISupport();

    void* sandbox_info = nullptr;
    // Security: Initialize sandbox.
    // In a real build, we'd link cef_sandbox.lib.
    CefScopedSandboxInfo scoped_sandbox;
    sandbox_info = scoped_sandbox.sandbox_info();

    CefMainArgs main_args(hInstance);

    // Parse command-line to determine process type
    CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
    command_line->InitFromString(::GetCommandLineW());
    std::string process_type = command_line->GetSwitchValue("type").ToString();

    CefRefPtr<CefApp> app;
    if (process_type == "renderer") {
        // If this is a renderer process, instantiate the renderer app implementation.
        // This is crucial for injecting V8 JS bindings (like cefQuery).
        app = new SecureRenderApp();
    } else if (process_type.empty()) {
        // Empty process type means this is the main browser process.
        app = new SecureBrowserApp();
    }
    // Other sub-processes (GPU, crashpad) will just use a null app or base CefApp.

    // CEF applications have multiple sub-processes (render, plugin, GPU, etc)
    // that share the same executable. This function checks the command-line and,
    // if this is a sub-process, executes the appropriate logic.
    int exit_code = CefExecuteProcess(main_args, app, sandbox_info);
    if (exit_code >= 0) {
        return exit_code;
    }

    // Settings for the browser process
    CefSettings settings;
    // Security: Enforce Sandbox
    settings.no_sandbox = false;

    // Initialize CEF.
    CefInitialize(main_args, settings, app, sandbox_info);

    // Run the CEF message loop. This will block until CefQuitMessageLoop() is called.
    CefRunMessageLoop();

    // Shut down CEF.
    CefShutdown();
#else
    std::cout << "Starting SecureBrowser on Windows (No CEF linked yet)..." << std::endl;
    SecureBrowserApp app;
#endif
    return 0;
}
#else
int main(int argc, char* argv[]) {
    std::cout << "Starting SecureBrowser (Linux mock build)..." << std::endl;

    SecureBrowserApp app;

    return 0;
}
#endif
