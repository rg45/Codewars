#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <iostream>

class TheBooleanOrderSolver {
public:
    size_t operator()(const std::string& s, const std::string& ops) {
        if (s.size() != ops.size() + 1)
            throw std::logic_error("Invalid input: " + s + ":" + ops);

        if (ops.size() > maxOpsSize)
            throw std::out_of_range("Too long expression: " + s + ":" + ops);

        std::string exprStr{s.front()};
        exprStr.reserve(s.size() + ops.size());
        for (size_t i = 0; i < ops.size(); ++i) {
            exprStr.push_back(ops[i]);
            exprStr.push_back(s[i + 1]);
        }
        getTotalCases(ops.size());
        return solve(Expression(exprStr), ops.size());
    }
private:

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

    class Expression {
    public:
        Expression() = default;
        explicit Expression(const std::string_view& input) {
            if (!input.empty()) {
                m = uint64_t(parseVal(input.front()));
                for (size_t i = 1; i < input.size(); i += 2) {
                    m += ((uint64_t(parseOps(input[i])) + (uint64_t(parseVal(input[i + 1])) << 2)) << leftBitCount((i - 1) / 2));
                }
            }
        }

        Ops Operator(size_t i) const { return Ops((m >> leftBitCount(i)) & 3); }
        Expression LeftOperand(size_t i) const { return Expression(m & ((1 << leftBitCount(i)) - 1)); }
        Expression RightOperand(size_t i) const { return Expression(m >> (leftBitCount(i) + 2)); }

        bool operator < (const Expression& rhs) const { return m < rhs.m; }
        bool operator == (const Expression& rhs) const { return m == rhs.m; }

    private:
        explicit Expression(uint64_t m) : m(m) {}
        static size_t leftBitCount(size_t i) { return i * 3 + 1; }

        uint64_t m{};
    };

    static constexpr size_t maxOpsSize = (sizeof(Expression) * CHAR_BIT - 1) / 3;

    class TotalCaseCalculator {
    public:
        TotalCaseCalculator() = default;
        TotalCaseCalculator(size_t opsSize) { ensureSize(opsSize); }

        size_t operator()(size_t opsSize) {
            ensureSize(opsSize);
            return m[opsSize];
        }

    private:
        void ensureSize(size_t opsSize) {
            if (m.size() < opsSize + 1) {
                m.reserve(opsSize + 1);
                while (m.size() < opsSize + 1) {
                    size_t t = 0;
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
        std::vector<size_t> m {1, 1};
    };

    TotalCaseCalculator getTotalCases;
    std::map<Expression, size_t> m_sols {
        { Expression("f"), 0},
        { Expression("t"), 1} };

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

    size_t solve(const Expression& expr, size_t opSize) {
        const auto it = m_sols.lower_bound(expr);
        if (it != m_sols.end() && it->first == expr)
            return it->second;

        size_t res = 0;
        for (size_t i = 0; i < opSize; ++i) {
            size_t lt = solve(expr.LeftOperand(i), i);
            size_t rt = solve(expr.RightOperand(i), opSize - i - 1);
            size_t lf = getTotalCases(i) - lt;
            size_t rf = getTotalCases(opSize - i - 1) - rt;
            switch (expr.Operator(i)) {
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
    CHECK(solve("tf", "^") == 1);
    CHECK(solve("tft", "^&") == 2);
    CHECK(solve("ttftff","|&^&&") == 16);
    CHECK(solve("ttftfftf","|&^&&||") == 339);
    CHECK(solve("ttftfftft","|&^&&||^") == 851);
    CHECK(solve("ttftfftftf", "|&^&&||^&") == 2434);
}
