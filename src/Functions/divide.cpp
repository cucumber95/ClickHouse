#include <Functions/FunctionFactory.h>
#include <Functions/FunctionBinaryArithmetic.h>

namespace DB
{
namespace ErrorCodes
{
    extern const int LOGICAL_ERROR;
}

template <typename A, typename B>
struct DivideFloatingImpl
{
    using ResultType = typename NumberTraits::ResultOfFloatingPointDivision<A, B>::Type;
    static const constexpr bool allow_fixed_string = false;
    static const constexpr bool allow_string_integer = false;

    template <typename Result = ResultType>
    static NO_SANITIZE_UNDEFINED Result apply(A a [[maybe_unused]], B b [[maybe_unused]])
    {
        return static_cast<Result>(a) / static_cast<Result>(b);
    }

#if USE_EMBEDDED_COMPILER
    static constexpr bool compilable = true;

    static llvm::Value * compile(llvm::IRBuilder<> & b, llvm::Value * left, llvm::Value * right, bool)
    {
        if (left->getType()->isIntegerTy())
            throw Exception(ErrorCodes::LOGICAL_ERROR, "DivideFloatingImpl expected a floating-point type");
        return b.CreateFDiv(left, right);
    }
#endif
};

template <typename A, typename B>
struct DivideFloatingOrNullImpl : DivideFloatingImpl<A, B>{};

struct NameDivide { static constexpr auto name = "divide"; };
using FunctionDivide = BinaryArithmeticOverloadResolver<DivideFloatingImpl, NameDivide>;

REGISTER_FUNCTION(Divide)
{
    factory.registerFunction<FunctionDivide>();
}

struct NameDivideOrNull { static constexpr auto name = "divideOrNull"; };
using FunctionDivideOrNull = BinaryArithmeticOverloadResolver<DivideFloatingOrNullImpl, NameDivideOrNull>;

REGISTER_FUNCTION(DivideOrNull)
{
    factory.registerFunction<FunctionDivideOrNull>();
}

}
