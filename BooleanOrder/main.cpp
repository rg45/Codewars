#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <iostream>

class TheBooleanOrderSolver {
public:
    uint64_t operator()(const std::string&, const std::string&) {
        return 0;
    }
private:
};

uint64_t solve(const std::string &s, const std::string &ops) {

    TheBooleanOrderSolver solver;
    return solver(s, ops);
}

TEST_CASE("basic_tests" "[The Boolean Order Tests]") {
    CHECK(solve("ttftfftftf", "|&^&&||^&") == 24341);
    /*
    CHECK(solve("tft","^&") == 2);
    CHECK(solve("ttftff","|&^&&") == 16);
    CHECK(solve("ttftfftf","|&^&&||") == 339);
    CHECK(solve("ttftfftft","|&^&&||^") == 851);
    CHECK(solve("ttftfftftf","|&^&&||^&") == 2434); */
}
