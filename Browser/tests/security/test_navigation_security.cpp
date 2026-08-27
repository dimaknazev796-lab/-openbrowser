#include <cassert>
#include <iostream>
#include "../../src/navigation/navigation_handler.h"

void test_security_url_blocking() {
    std::cout << "[Security Test] Testing URL scheme blocking..." << std::endl;

    // Dangerous schemes should be blocked
    assert(NavigationHandler::IsSecureUrl("javascript:alert(1)") == false);
    assert(NavigationHandler::IsSecureUrl("data:text/html,<h1>test</h1>") == false);
    assert(NavigationHandler::IsSecureUrl("file:///C:/Windows/System32/cmd.exe") == false);
    assert(NavigationHandler::IsSecureUrl("vbscript:msgbox(\"test\")") == false);

    // Safe schemes should be allowed
    assert(NavigationHandler::IsSecureUrl("https://www.google.com") == true);
    assert(NavigationHandler::IsSecureUrl("http://example.com") == true);

    std::cout << "[Security Test] URL scheme blocking PASSED." << std::endl;
}

void test_omnibox_parsing() {
    std::cout << "[Security Test] Testing Omnibox Parsing heuristic..." << std::endl;

    // Explicit scheme
    assert(NavigationHandler::ParseOmniboxInput("https://github.com") == "https://github.com");

    // Implied URL (contains dot, no spaces)
    assert(NavigationHandler::ParseOmniboxInput("example.com") == "https://example.com");

    // Dangerous attempt via implied URL (should just pass through so IsSecureUrl blocks it later)
    assert(NavigationHandler::ParseOmniboxInput("javascript:alert(1)") == "javascript:alert(1)");

    // Search query
    assert(NavigationHandler::ParseOmniboxInput("how to build a browser") == "https://www.google.com/search?q=how+to+build+a+browser");

    std::cout << "[Security Test] Omnibox Parsing PASSED." << std::endl;
}

int main() {
    test_security_url_blocking();
    test_omnibox_parsing();

    std::cout << "All Security Tests Passed Successfully!" << std::endl;
    return 0;
}
