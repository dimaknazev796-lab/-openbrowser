# WIN32 property tells CMake/MSVC that this is a Windows GUI application,
# ensuring the linker correctly looks for wWinMain instead of standard main.
add_executable(SecureBrowser WIN32 main.cpp app/browser_app.cpp app/browser_client.cpp tabs/tab_manager.cpp navigation/navigation_handler.cpp renderer/render_app.cpp)

# When CEF is fully integrated and downloaded via FetchContent on the host Windows machine:
# target_link_libraries(SecureBrowser PRIVATE libcef_dll_wrapper)
