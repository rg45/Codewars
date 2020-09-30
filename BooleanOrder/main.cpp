#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <iostream>

class TheBooleanOrderSolver {
public:
    uint64_t operator()(const std::string& s, const std::string& ops) {
        if (s.size() != ops.size() + 1)
            throw std::logic_error("Invalid input: " + s + ":" + ops);

        if (ops.size() > maxOpsSize)
            throw std::out_of_range("Too long expression: " + s + ":" + ops);

        uint64_t expr = uint64_t(parseVal(s[0]));

        for (size_t i = 0; i < ops.size(); ++i) {
            expr += uint64_t(parseOps(ops[i])) << (i * 3 + 1);
            expr += uint64_t(parseVal(s[i + 1])) << (i * 3 + 3);
        }
        getTotalCases(ops.size());
        return solve(expr, ops.size());
    }
private:
    static constexpr size_t maxOpsSize = (sizeof(uint64_t) * CHAR_BIT - 1) / 3;
    enum class Val : unsigned {
        False = 0,
        True = 1,
    };
    enum class Ops : unsigned {
        None = 0,
        And = 1,
        Or = 2,
        Xor = 3,
    };

    class TotalCaseCalculator {
    public:
        TotalCaseCalculator() = default;
        TotalCaseCalculator(size_t opsSize) { ensureSize(opsSize); }

        uint64_t operator()(size_t opsSize) {
            ensureSize(opsSize);
            return m[opsSize];
        }

    private:
        void ensureSize(size_t opsSize) {
            if (m.size() < opsSize + 1) {
                m.reserve(opsSize + 1);
                while (m.size() < opsSize + 1) {
                    uint64_t t = 0;
                    size_t i = 0, j = m.size() - 1;
                    while (i < j) {
                        t += m[i++] * m[j--] * 2;
                    }
                    if (i == j) {
                        t += m[i] * m[j];
                    }
                    m.push_back(t);
                }
            }
        }
        std::vector<uint64_t> m {1, 1};
    };

    TotalCaseCalculator getTotalCases;
    std::map<uint64_t, uint64_t> m_sols {
        {uint64_t(Val::False), 0},
        {uint64_t(Val::True), 1} };

    static Val parseVal(char c) {
        switch (c) {
            case 'f': return Val::False;
            case 't': return Val::True;
            default: throw std::logic_error(std::string("Invalid value symbol: ") + c);
        }
    }
    static Ops parseOps(char c) {
        switch (c) {
            case '&': return Ops::And;
            case '|': return Ops::Or;
            case '^': return Ops::Xor;
            default: throw std::logic_error(std::string("Invalid op symbol: ") + c);
        }
    }

    uint64_t solve(uint64_t expr, size_t opSize) {
        const auto it = m_sols.lower_bound(expr);
        if (it != m_sols.end() && it->first == expr)
            return it->second;

        uint64_t res = 0;
        for (size_t i = 0; i < opSize; ++i) {
            size_t lBits = i * 3 + 1;
            uint64_t lExpr = expr & ((1 << lBits) - 1);
            uint64_t rExpr = expr >> (lBits + 2);
            uint64_t lt = solve(lExpr, i);
            uint64_t rt = solve(rExpr, opSize - i - 1);
            uint64_t lf = getTotalCases(i) - lt;
            uint64_t rf = getTotalCases(opSize - i - 1) - rt;
            switch (Ops((expr >> lBits) & 3)) {
                case Ops::And:
                    res += lt * rt;
                    break;

                case Ops::Or:
                    res += lt * rt + lt * rf + lf * rt;
                    break;

                case Ops::Xor:
                    res += lt * rf + lf * rt;
                    break;
            }
        }
        m_sols.insert(it, { expr, res });
        return res;
    }
};

uint64_t solve(const std::string &s, const std::string &ops) {

    TheBooleanOrderSolver solver;
    return solver(s, ops);
}

TEST_CASE("basic_tests" "[The Boolean Order Tests]") {
    CHECK(solve("tft", "^&") == 2);
    CHECK(solve("ttftff","|&^&&") == 16);
    CHECK(solve("ttftfftf","|&^&&||") == 339);
    CHECK(solve("ttftfftft","|&^&&||^") == 851);
    CHECK(solve("ttftfftftf", "|&^&&||^&") == 2434);     /*
*/
}
