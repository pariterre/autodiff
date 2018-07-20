// C++ includes
#include <cmath>
#include <memory>

/// autodiff namespace where @ref var and @ref grad are defined.
namespace autodiff {

namespace internal {

struct Expr;
struct ParameterExpr;
struct VariableExpr;
struct ConstantExpr;
struct UnaryExpr;
struct NegativeExpr;
struct BinaryExpr;
struct AddExpr;
struct SubExpr;
struct MulExpr;
struct DivExpr;
struct SinExpr;
struct CosExpr;
struct TanExpr;
struct ArcSinExpr;
struct ArcCosExpr;
struct ArcTanExpr;
struct ExpExpr;
struct LogExpr;
struct Log10Expr;
struct PowExpr;
struct SqrtExpr;
struct AbsExpr;

using ExprPtr = std::shared_ptr<Expr>;

//------------------------------------------------------------------------------
// CONVENIENT FUNCTIONS (DECLARATION ONLY)
//------------------------------------------------------------------------------
ExprPtr constant(double val);

//------------------------------------------------------------------------------
// ARITHMETIC OPERATORS (DECLARATION ONLY)
//------------------------------------------------------------------------------
ExprPtr operator+(const ExprPtr& r);
ExprPtr operator-(const ExprPtr& r);

ExprPtr operator+(const ExprPtr& l, const ExprPtr& r);
ExprPtr operator-(const ExprPtr& l, const ExprPtr& r);
ExprPtr operator*(const ExprPtr& l, const ExprPtr& r);
ExprPtr operator/(const ExprPtr& l, const ExprPtr& r);

ExprPtr operator+(double l, const ExprPtr& r);
ExprPtr operator-(double l, const ExprPtr& r);
ExprPtr operator*(double l, const ExprPtr& r);
ExprPtr operator/(double l, const ExprPtr& r);

ExprPtr operator+(const ExprPtr& l, double r);
ExprPtr operator-(const ExprPtr& l, double r);
ExprPtr operator*(const ExprPtr& l, double r);
ExprPtr operator/(const ExprPtr& l, double r);

//------------------------------------------------------------------------------
// TRIGONOMETRIC FUNCTIONS (DECLARATION ONLY)
//------------------------------------------------------------------------------
ExprPtr sin(const ExprPtr& x);
ExprPtr cos(const ExprPtr& x);
ExprPtr tan(const ExprPtr& x);
ExprPtr asin(const ExprPtr& x);
ExprPtr acos(const ExprPtr& x);
ExprPtr atan(const ExprPtr& x);

//------------------------------------------------------------------------------
// HYPERBOLIC FUNCTIONS (DECLARATION ONLY)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// EXPONENTIAL AND LOGARITHMIC FUNCTIONS (DECLARATION ONLY)
//------------------------------------------------------------------------------
ExprPtr exp(const ExprPtr& x);
ExprPtr log(const ExprPtr& x);
ExprPtr log10(const ExprPtr& x);

//------------------------------------------------------------------------------
// POWER FUNCTIONS (DECLARATION ONLY)
//------------------------------------------------------------------------------
ExprPtr pow(const ExprPtr& l, const ExprPtr& r);
ExprPtr pow(double l, const ExprPtr& r);
ExprPtr pow(const ExprPtr& l, double r);
ExprPtr sqrt(const ExprPtr& x);

//------------------------------------------------------------------------------
// OTHER FUNCTIONS (DECLARATION ONLY)
//------------------------------------------------------------------------------
ExprPtr abs(const ExprPtr& x);
ExprPtr abs2(const ExprPtr& x);
ExprPtr conj(const ExprPtr& x);
ExprPtr real(const ExprPtr& x);
ExprPtr imag(const ExprPtr& x);

//------------------------------------------------------------------------------
// COMPARISON OPERATORS (DECLARATION ONLY)
//------------------------------------------------------------------------------
bool operator==(const ExprPtr& l, const ExprPtr& r);
bool operator!=(const ExprPtr& l, const ExprPtr& r);
bool operator<=(const ExprPtr& l, const ExprPtr& r);
bool operator>=(const ExprPtr& l, const ExprPtr& r);
bool operator<(const ExprPtr& l, const ExprPtr& r);
bool operator>(const ExprPtr& l, const ExprPtr& r);

bool operator==(double l, const ExprPtr& r);
bool operator!=(double l, const ExprPtr& r);
bool operator<=(double l, const ExprPtr& r);
bool operator>=(double l, const ExprPtr& r);
bool operator<(double l, const ExprPtr& r);
bool operator>(double l, const ExprPtr& r);

bool operator==(const ExprPtr& l, double r);
bool operator!=(const ExprPtr& l, double r);
bool operator<=(const ExprPtr& l, double r);
bool operator>=(const ExprPtr& l, double r);
bool operator<(const ExprPtr& l, double r);
bool operator>(const ExprPtr& l, double r);

struct Expr
{
    /// The numerical value of this expression.
    double val;

    /// Construct an Expr object with given numerical value.
    explicit Expr(double val) : val(val) {}

    /// Return the gradient value of this expression with respect to given variable.
    virtual double grad(const ExprPtr& param) const = 0;

    /// Return the gradient expression of this expression with respect to given variable.
    virtual ExprPtr gradx(const ExprPtr& param) const = 0;
};

struct ParameterExpr : Expr
{
    using Expr::Expr;

    virtual double grad(const ExprPtr& param) const
    {
        return this == param.get();
    }

    virtual ExprPtr gradx(const ExprPtr& param) const
    {
        return constant(this == param.get());
    }
};

struct VariableExpr : Expr
{
    ExprPtr expr;

    VariableExpr(const ExprPtr& expr) : Expr(expr->val), expr(expr) {}

    virtual double grad(const ExprPtr& param) const
    {
        return this == param.get() ? 1.0 : expr->grad(param);
    }

    virtual ExprPtr gradx(const ExprPtr& param) const
    {
        return this == param.get() ? constant(1.0) : expr->gradx(param);
    }
};

struct ConstantExpr : Expr
{
    using Expr::Expr;

    virtual double grad(const ExprPtr& param) const
    {
        return 0.0;
    }

    virtual ExprPtr gradx(const ExprPtr& param) const
    {
        return constant(0.0);
    }
};

struct UnaryExpr : Expr
{
    ExprPtr x;

    UnaryExpr(double val, const ExprPtr& x) : Expr(val), x(x) {}
};

struct NegativeExpr : UnaryExpr
{
    using UnaryExpr::UnaryExpr;

    virtual double grad(const ExprPtr& param) const
    {
        return -x->grad(param);
    }

    virtual ExprPtr gradx(const ExprPtr& param) const
    {
        return -x->gradx(param);
    }
};

struct BinaryExpr : Expr
{
    ExprPtr l, r;

    BinaryExpr(double val, const ExprPtr& l, const ExprPtr& r) : Expr(val), l(l), r(r) {}
};

struct AddExpr : BinaryExpr
{
    using BinaryExpr::BinaryExpr;

    virtual double grad(const ExprPtr& param) const
    {
        return l->grad(param) + r->grad(param);
    }

    virtual ExprPtr gradx(const ExprPtr& param) const
    {
        return l->gradx(param) + r->gradx(param);
    }
};

struct SubExpr : BinaryExpr
{
    using BinaryExpr::BinaryExpr;

    virtual double grad(const ExprPtr& param) const
    {
        return l->grad(param) - r->grad(param);
    }

    virtual ExprPtr gradx(const ExprPtr& param) const
    {
        return l->gradx(param) - r->gradx(param);
    }
};

struct MulExpr : BinaryExpr
{
    using BinaryExpr::BinaryExpr;

    virtual double grad(const ExprPtr& param) const
    {
        return l->grad(param) * r->val + l->val * r->grad(param);
    }

    virtual ExprPtr gradx(const ExprPtr& param) const
    {
        return l->gradx(param) * r + l * r->gradx(param);
    }
};

struct DivExpr : BinaryExpr
{
    using BinaryExpr::BinaryExpr;

    virtual double grad(const ExprPtr& param) const
    {
        return ( l->grad(param) * r->val - l->val * r->grad(param) ) / (r->val * r->val);
    }

    virtual ExprPtr gradx(const ExprPtr& param) const
    {
        return ( l->gradx(param) * r - l * r->gradx(param) ) / (r * r);
    }
};

struct SinExpr : UnaryExpr
{
    double aux;

    SinExpr(double val, const ExprPtr& x) : UnaryExpr(val, x), aux(std::cos(x->val)) {}

    virtual double grad(const ExprPtr& param) const
    {
        return aux * x->grad(param);
    }

    virtual ExprPtr gradx(const ExprPtr& param) const
    {
        return cos(x) * x->gradx(param);
    }
};

struct CosExpr : UnaryExpr
{
    double aux;

    CosExpr(double val, const ExprPtr& x) : UnaryExpr(val, x), aux(-std::sin(x->val)) {}

    virtual double grad(const ExprPtr& param) const
    {
        return aux * x->grad(param);
    }

    virtual ExprPtr gradx(const ExprPtr& param) const
    {
        return -sin(x) * x->gradx(param);
    }
};

struct TanExpr : UnaryExpr
{
    double aux;

    TanExpr(double val, const ExprPtr& x) : UnaryExpr(val, x), aux(1.0 / std::cos(x->val)) {}

    virtual double grad(const ExprPtr& param) const
    {
        return aux * aux * x->grad(param);
    }

    virtual ExprPtr gradx(const ExprPtr& param) const
    {
        return x->gradx(param) / (cos(x) * cos(x));
    }
};

struct ArcSinExpr : UnaryExpr
{
    double aux;

    ArcSinExpr(double val, const ExprPtr& x) : UnaryExpr(val, x), aux(1.0 / std::sqrt(1.0 - x->val * x->val)) {}

    virtual double grad(const ExprPtr& param) const
    {
        return aux * x->grad(param);
    }

    virtual ExprPtr gradx(const ExprPtr& param) const
    {
        return 1.0 / sqrt(1.0 - x * x) * x->gradx(param);
    }
};

struct ArcCosExpr : UnaryExpr
{
    double aux;

    ArcCosExpr(double val, const ExprPtr& x) : UnaryExpr(val, x), aux(-1.0 / std::sqrt(1.0 - x->val * x->val)) {}

    virtual double grad(const ExprPtr& param) const
    {
        return aux * x->grad(param);
    }

    virtual ExprPtr gradx(const ExprPtr& param) const
    {
        return -1.0 / sqrt(1.0 - x * x) * x->gradx(param);
    }
};

struct ArcTanExpr : UnaryExpr
{
    double aux;

    ArcTanExpr(double val, const ExprPtr& x) : UnaryExpr(val, x), aux(1.0 / (1.0 + x->val * x->val)) {}

    virtual double grad(const ExprPtr& param) const
    {
        return aux * x->grad(param);
    }

    virtual ExprPtr gradx(const ExprPtr& param) const
    {
        return 1.0 / (1.0 + x * x) * x->gradx(param);
    }
};

struct ExpExpr : UnaryExpr
{
    using UnaryExpr::UnaryExpr;

    virtual double grad(const ExprPtr& param) const
    {
        return val * x->grad(param);
    }

    virtual ExprPtr gradx(const ExprPtr& param) const
    {
        return exp(x) * x->gradx(param);
    }
};

struct LogExpr : UnaryExpr
{
    using UnaryExpr::UnaryExpr;

    virtual double grad(const ExprPtr& param) const
    {
        return x->grad(param) / x->val;
    }

    virtual ExprPtr gradx(const ExprPtr& param) const
    {
        return x->gradx(param) / x;
    }
};

struct Log10Expr : UnaryExpr
{
    static constexpr double ln10 = 2.3025850929940456840179914546843;

    double aux;

    Log10Expr(double val, const ExprPtr& x) : UnaryExpr(val, x), aux(1.0 / (ln10 * x->val)) {}

    virtual double grad(const ExprPtr& param) const
    {
        return aux * x->grad(param);
    }

    virtual ExprPtr gradx(const ExprPtr& param) const
    {
        return 1.0 / (ln10 * x) * x->gradx(param);
    }
};

struct PowExpr : BinaryExpr
{
    double log_l;

    PowExpr(double val, const ExprPtr& l, const ExprPtr& r) : BinaryExpr(val, l, r), log_l(std::log(l->val)) {}

    virtual double grad(const ExprPtr& param) const
    {
        return ( log_l * r->grad(param) + r->val / l->val * l->grad(param) ) * val;
    }

    virtual ExprPtr gradx(const ExprPtr& param) const
    {
        return ( log(l) * r->gradx(param) + r/l * l->gradx(param) ) * pow(l, r);
    }
};

struct SqrtExpr : UnaryExpr
{
    double aux;

    SqrtExpr(double val, const ExprPtr& x) : UnaryExpr(val, x), aux(-0.5 / std::sqrt(x->val)) {}

    virtual double grad(const ExprPtr& param) const
    {
        return aux * x->grad(param);
    }

    virtual ExprPtr gradx(const ExprPtr& param) const
    {
        return aux * x->gradx(param);
    }
};

struct AbsExpr : UnaryExpr
{
    double aux;

    AbsExpr(double val, const ExprPtr& x) : UnaryExpr(val, x), aux(x->val / std::abs(x->val)) {}

    virtual double grad(const ExprPtr& param) const
    {
        return aux * x->grad(param);
    }

    virtual ExprPtr gradx(const ExprPtr& param) const
    {
        return x / abs(x) * x->gradx(param);
    }
};

//------------------------------------------------------------------------------
// CONVENIENT FUNCTIONS
//------------------------------------------------------------------------------
inline ExprPtr constant(double val) { return std::make_shared<ConstantExpr>(val); }

//------------------------------------------------------------------------------
// ARITHMETIC OPERATORS
//------------------------------------------------------------------------------
inline ExprPtr operator+(const ExprPtr& r) { return r; }
inline ExprPtr operator-(const ExprPtr& r) { return std::make_shared<NegativeExpr>(-r->val, r); }

inline ExprPtr operator+(const ExprPtr& l, const ExprPtr& r) { return std::make_shared<AddExpr>(l->val + r->val, l, r); }
inline ExprPtr operator-(const ExprPtr& l, const ExprPtr& r) { return std::make_shared<SubExpr>(l->val - r->val, l, r); }
inline ExprPtr operator*(const ExprPtr& l, const ExprPtr& r) { return std::make_shared<MulExpr>(l->val * r->val, l, r); }
inline ExprPtr operator/(const ExprPtr& l, const ExprPtr& r) { return std::make_shared<DivExpr>(l->val / r->val, l, r); }

inline ExprPtr operator+(double l, const ExprPtr& r) { return constant(l) + r; }
inline ExprPtr operator-(double l, const ExprPtr& r) { return constant(l) - r; }
inline ExprPtr operator*(double l, const ExprPtr& r) { return constant(l) * r; }
inline ExprPtr operator/(double l, const ExprPtr& r) { return constant(l) / r; }

inline ExprPtr operator+(const ExprPtr& l, double r) { return l + constant(r); }
inline ExprPtr operator-(const ExprPtr& l, double r) { return l - constant(r); }
inline ExprPtr operator*(const ExprPtr& l, double r) { return l * constant(r); }
inline ExprPtr operator/(const ExprPtr& l, double r) { return l / constant(r); }

//------------------------------------------------------------------------------
// TRIGONOMETRIC FUNCTIONS
//------------------------------------------------------------------------------
inline ExprPtr sin(const ExprPtr& x) { return std::make_shared<SinExpr>(std::sin(x->val), x); }
inline ExprPtr cos(const ExprPtr& x) { return std::make_shared<CosExpr>(std::cos(x->val), x); }
inline ExprPtr tan(const ExprPtr& x) { return std::make_shared<TanExpr>(std::tan(x->val), x); }
inline ExprPtr asin(const ExprPtr& x) { return std::make_shared<ArcSinExpr>(std::asin(x->val), x); }
inline ExprPtr acos(const ExprPtr& x) { return std::make_shared<ArcCosExpr>(std::acos(x->val), x); }
inline ExprPtr atan(const ExprPtr& x) { return std::make_shared<ArcTanExpr>(std::atan(x->val), x); }

//------------------------------------------------------------------------------
// HYPERBOLIC FUNCTIONS
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// EXPONENTIAL AND LOGARITHMIC FUNCTIONS
//------------------------------------------------------------------------------
inline ExprPtr exp(const ExprPtr& x) { return std::make_shared<ExpExpr>(std::exp(x->val), x); }
inline ExprPtr log(const ExprPtr& x) { return std::make_shared<LogExpr>(std::log(x->val), x); }
inline ExprPtr log10(const ExprPtr& x) { return std::make_shared<Log10Expr>(std::log10(x->val), x); }

//------------------------------------------------------------------------------
// POWER FUNCTIONS
//------------------------------------------------------------------------------
inline ExprPtr pow(const ExprPtr& l, const ExprPtr& r) { return std::make_shared<PowExpr>(std::pow(l->val, r->val), l, r); }
inline ExprPtr pow(double l, const ExprPtr& r) { return pow(constant(l), r); }
inline ExprPtr pow(const ExprPtr& l, double r) { return pow(l, constant(r)); }
inline ExprPtr sqrt(const ExprPtr& x) { return std::make_shared<SqrtExpr>(std::sqrt(x->val), x); }

//------------------------------------------------------------------------------
// OTHER FUNCTIONS
//------------------------------------------------------------------------------
inline ExprPtr abs(const ExprPtr& x) { return std::make_shared<AbsExpr>(std::abs(x->val), x); }
inline ExprPtr abs2(const ExprPtr& x) { return x * x; }
inline ExprPtr conj(const ExprPtr& x) { return x; }
inline ExprPtr real(const ExprPtr& x) { return x; }
inline ExprPtr imag(const ExprPtr& x) { return constant(0.0); }

//------------------------------------------------------------------------------
// COMPARISON OPERATORS
//------------------------------------------------------------------------------
inline bool operator==(const ExprPtr& l, const ExprPtr& r) { return l->val == r->val; }
inline bool operator!=(const ExprPtr& l, const ExprPtr& r) { return l->val != r->val; }
inline bool operator<=(const ExprPtr& l, const ExprPtr& r) { return l->val <= r->val; }
inline bool operator>=(const ExprPtr& l, const ExprPtr& r) { return l->val >= r->val; }
inline bool operator<(const ExprPtr& l, const ExprPtr& r) { return l->val < r->val; }
inline bool operator>(const ExprPtr& l, const ExprPtr& r) { return l->val > r->val; }

inline bool operator==(double l, const ExprPtr& r) { return l == r->val; }
inline bool operator!=(double l, const ExprPtr& r) { return l != r->val; }
inline bool operator<=(double l, const ExprPtr& r) { return l <= r->val; }
inline bool operator>=(double l, const ExprPtr& r) { return l >= r->val; }
inline bool operator<(double l, const ExprPtr& r) { return l < r->val; }
inline bool operator>(double l, const ExprPtr& r) { return l > r->val; }

inline bool operator==(const ExprPtr& l, double r) { return l->val == r; }
inline bool operator!=(const ExprPtr& l, double r) { return l->val != r; }
inline bool operator<=(const ExprPtr& l, double r) { return l->val <= r; }
inline bool operator>=(const ExprPtr& l, double r) { return l->val >= r; }
inline bool operator<(const ExprPtr& l, double r) { return l->val < r; }
inline bool operator>(const ExprPtr& l, double r) { return l->val > r; }

} // namespace internal

using namespace internal;

/// The autodiff variable type used for automatic differentiation.
struct var
{
    /// The pointer to the expression tree of variable operations
    ExprPtr expr;

    /// Construct a default var object variable
    var() : var(0.0) {}

    /// Construct a var object variable with given value
    var(double val) : expr(std::make_shared<ParameterExpr>(val)) {}

    /// Construct a var object variable with given expression
    var(const ExprPtr& expr) : expr(std::make_shared<VariableExpr>(expr)) {}

    /// Implicitly convert this var object variable into an expression pointer
    operator ExprPtr() const { return expr; }
};

/// Return the value of a variable x.
inline double val(const var& x)
{
    return x.expr->val;
}

/// Return the derivative of a variable y with respect to variable x.
inline double grad(const var& y, const var& x)
{
    return y.expr->grad(x.expr);
}

/// Return the derivative expression of a variable y with respect to variable x.
inline var gradx(const var& y, const var& x)
{
    return y.expr->gradx(x.expr);
}

/// Output a var object variable to the output stream.
inline std::ostream& operator<<(std::ostream& out, const var& x)
{
    out << autodiff::val(x);
    return out;
}

} // namespace autodiff

//------------------------------------------------------------------------------
// SUPPORT FOR EIGEN MATRICES AND VECTORS OF AUTODIFF::VAR
//------------------------------------------------------------------------------
#ifdef AUTODIFF_ENABLE_EIGEN_SUPPORT

namespace Eigen {

template<typename T>
struct NumTraits;

template<> struct NumTraits<autodiff::var> : NumTraits<double> // permits to get the epsilon, dummy_precision, lowest, highest functions
{
    typedef autodiff::var Real;
    typedef autodiff::var NonInteger;
    typedef autodiff::var Nested;
    enum
    {
        IsComplex = 0,
        IsInteger = 0,
        IsSigned = 1,
        RequireInitialization = 1,
        ReadCost = 1,
        AddCost = 3,
        MulCost = 3
    };
};

#define EIGEN_MAKE_TYPEDEFS(Type, TypeSuffix, Size, SizeSuffix)   \
typedef Matrix<Type, Size, Size, 0, Size, Size> Matrix##SizeSuffix##TypeSuffix;  \
typedef Matrix<Type, Size, 1, 0, Size, 1>       Vector##SizeSuffix##TypeSuffix;  \
typedef Matrix<Type, 1, Size, 1, 1, Size>       RowVector##SizeSuffix##TypeSuffix;

#define EIGEN_MAKE_FIXED_TYPEDEFS(Type, TypeSuffix, Size)         \
typedef Matrix<Type, Size, -1, 0, Size, -1> Matrix##Size##X##TypeSuffix;  \
typedef Matrix<Type, -1, Size, 0, -1, Size> Matrix##X##Size##TypeSuffix;

#define EIGEN_MAKE_TYPEDEFS_ALL_SIZES(Type, TypeSuffix) \
EIGEN_MAKE_TYPEDEFS(Type, TypeSuffix, 2, 2) \
EIGEN_MAKE_TYPEDEFS(Type, TypeSuffix, 3, 3) \
EIGEN_MAKE_TYPEDEFS(Type, TypeSuffix, 4, 4) \
EIGEN_MAKE_TYPEDEFS(Type, TypeSuffix, -1, X) \
EIGEN_MAKE_FIXED_TYPEDEFS(Type, TypeSuffix, 2) \
EIGEN_MAKE_FIXED_TYPEDEFS(Type, TypeSuffix, 3) \
EIGEN_MAKE_FIXED_TYPEDEFS(Type, TypeSuffix, 4)

EIGEN_MAKE_TYPEDEFS_ALL_SIZES(autodiff::var, v)

#undef EIGEN_MAKE_TYPEDEFS_ALL_SIZES
#undef EIGEN_MAKE_TYPEDEFS
#undef EIGEN_MAKE_FIXED_TYPEDEFS

} // namespace Eigen

namespace autodiff {

//------------------------------------------------------------------------------
// ARITHMETIC OPERATORS (DEFINED FOR ARGUMENTS OF TYPE var)
//------------------------------------------------------------------------------
inline ExprPtr operator+(const var& r) { return r.expr; }
inline ExprPtr operator-(const var& r) { return -r.expr; }

inline ExprPtr operator+(const var& l, const var& r) { return l.expr + r.expr; }
inline ExprPtr operator-(const var& l, const var& r) { return l.expr - r.expr; }
inline ExprPtr operator*(const var& l, const var& r) { return l.expr * r.expr; }
inline ExprPtr operator/(const var& l, const var& r) { return l.expr / r.expr; }

inline ExprPtr operator+(double l, const var& r) { return l + r.expr; }
inline ExprPtr operator-(double l, const var& r) { return l - r.expr; }
inline ExprPtr operator*(double l, const var& r) { return l * r.expr; }
inline ExprPtr operator/(double l, const var& r) { return l / r.expr; }

inline ExprPtr operator+(const var& l, double r) { return l.expr + r; }
inline ExprPtr operator-(const var& l, double r) { return l.expr - r; }
inline ExprPtr operator*(const var& l, double r) { return l.expr * r; }
inline ExprPtr operator/(const var& l, double r) { return l.expr / r; }

//------------------------------------------------------------------------------
// TRIGONOMETRIC FUNCTIONS (DEFINED FOR ARGUMENTS OF TYPE var)
//------------------------------------------------------------------------------
inline ExprPtr sin(const var& x) { return sin(x.expr); }
inline ExprPtr cos(const var& x) { return cos(x.expr); }
inline ExprPtr tan(const var& x) { return tan(x.expr); }
inline ExprPtr asin(const var& x) { return asin(x.expr); }
inline ExprPtr acos(const var& x) { return acos(x.expr); }
inline ExprPtr atan(const var& x) { return atan(x.expr); }

//------------------------------------------------------------------------------
// HYPERBOLIC FUNCTIONS (DEFINED FOR ARGUMENTS OF TYPE var)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// EXPONENTIAL AND LOGARITHMIC FUNCTIONS (DEFINED FOR ARGUMENTS OF TYPE var)
//------------------------------------------------------------------------------
inline ExprPtr exp(const var& x) { return exp(x.expr); }
inline ExprPtr log(const var& x) { return log(x.expr); }
inline ExprPtr log10(const var& x) { return log10(x.expr); }

//------------------------------------------------------------------------------
// POWER FUNCTIONS (DEFINED FOR ARGUMENTS OF TYPE var)
//------------------------------------------------------------------------------
inline ExprPtr pow(const var& l, const var& r) { return pow(l.expr, r.expr); }
inline ExprPtr pow(double l, const var& r) { return pow(l, r.expr); }
inline ExprPtr pow(const var& l, double r) { return pow(l.expr, r); }
inline ExprPtr sqrt(const var& x) { return sqrt(x.expr); }

//------------------------------------------------------------------------------
// OTHER FUNCTIONS (DEFINED FOR ARGUMENTS OF TYPE var)
//------------------------------------------------------------------------------
inline ExprPtr abs(const var& x) { return abs(x.expr); }
inline ExprPtr abs2(const var& x) { return abs2(x.expr); }
inline ExprPtr conj(const var& x) { return conj(x.expr); }
inline ExprPtr real(const var& x) { return real(x.expr); }
inline ExprPtr imag(const var& x) { return imag(x.expr); }

//------------------------------------------------------------------------------
// COMPARISON OPERATORS (DEFINED FOR ARGUMENTS OF TYPE var)
//------------------------------------------------------------------------------
inline bool operator==(const var& l, const var& r) { return l.expr == r.expr; }
inline bool operator!=(const var& l, const var& r) { return l.expr != r.expr; }
inline bool operator<=(const var& l, const var& r) { return l.expr <= r.expr; }
inline bool operator>=(const var& l, const var& r) { return l.expr >= r.expr; }
inline bool operator<(const var& l, const var& r) { return l.expr < r.expr; }
inline bool operator>(const var& l, const var& r) { return l.expr > r.expr; }

inline bool operator==(double l, const var& r) { return l == r.expr; }
inline bool operator!=(double l, const var& r) { return l != r.expr; }
inline bool operator<=(double l, const var& r) { return l <= r.expr; }
inline bool operator>=(double l, const var& r) { return l >= r.expr; }
inline bool operator<(double l, const var& r) { return l < r.expr; }
inline bool operator>(double l, const var& r) { return l > r.expr; }

inline bool operator==(const var& l, double r) { return l.expr == r; }
inline bool operator!=(const var& l, double r) { return l.expr != r; }
inline bool operator<=(const var& l, double r) { return l.expr <= r; }
inline bool operator>=(const var& l, double r) { return l.expr >= r; }
inline bool operator<(const var& l, double r) { return l.expr < r; }
inline bool operator>(const var& l, double r) { return l.expr > r; }

namespace internal {

/// Auxiliary function to calculate gradient of y with respect to x
inline void grad(const var& y, const var& x, double& dydx)
{
    dydx = y.expr->grad(x.expr);
}

/// Auxiliary function to construct gradient expression of y with respect to x
inline void grad(const var& y, const var& x, var& dydx)
{
    dydx = y.expr->gradx(x.expr);
}

/// Auxiliary template function to calculate gradient vector or gradient expression of y with respect to x
template<typename R>
R grad(const var& y, const Eigen::Ref<VectorXv>& x)
{
    const auto n = x.size();
    R dydx(n);
    for(auto i = 0; i < n; ++i)
        grad(y, x[i], dydx[i]);
    return dydx;
}

/// Auxiliary template function to calculate Jacobian matrix or Jacobian expression of y with respect to x
template<typename R>
R grad(const Eigen::Ref<VectorXv>& y, const Eigen::Ref<VectorXv>& x)
{
    const auto m = y.size();
    const auto n = x.size();
    R dydx(m, n);
    for(auto i = 0; i < m; ++i)
        for(auto j = 0; j < n; ++j)
            grad(y[i], x[j], dydx(i, j));
    return dydx;
}

} // namespace internal

/// Return the gradient of variable y with respect to variables x.
inline RowVectorXd grad(const var& y, const Eigen::Ref<VectorXv>& x)
{
    return internal::grad<RowVectorXd>(y, x);
}

/// Return the gradient expression of variable y with respect to variables x.
inline RowVectorXv gradx(const var& y, const Eigen::Ref<VectorXv>& x)
{
    return internal::grad<RowVectorXv>(y, x);
}

/// Return the Jacobian of variables y with respect to variables x.
inline MatrixXd grad(const Eigen::Ref<VectorXv>& y, const Eigen::Ref<VectorXv>& x)
{
    return internal::grad<MatrixXd>(y, x);
}

/// Return the Jacobian expression of variables y with respect to variables x.
inline MatrixXv gradx(const Eigen::Ref<VectorXv>& y, const Eigen::Ref<VectorXv>& x)
{
    return internal::grad<MatrixXv>(y, x);
}

} // namespace autodiff

#endif // AUTODIFF_ENABLE_EIGEN_SUPPORT
