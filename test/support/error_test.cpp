#include <support/error.hpp>

#include <catch2/catch.hpp>

#include <winerror.h>

TEST_CASE("throwing formatted exception", "[error]")
{
    const HRESULT result_err = E_ABORT;
    CHECK_THROWS_WITH(invio::throw_if_error<invio::initialization_failed>(
                          result_err, "Initialization failed (number={})", 100),
                      "Initialization failed (number=100)\nOperation aborted");

    const std::error_code err_code{E_ABORT, std::system_category()};
    CHECK_THROWS_WITH(invio::throw_if_error<invio::initialization_failed>(
                          err_code, "Initialization failed (number={})", 100),
                      "Initialization failed (number=100)\nOperation aborted");
}