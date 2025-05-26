
using namespace std;

class CPolynomial
{
public:
    CPolynomial() {
        coeffs.push_back(0.0);
    }
    double & operator[](size_t index) {
        if (index >= coeffs.size())
            coeffs.resize(index + 1, 0.0);
        return coeffs[index];
    }
    const double & operator[](size_t index) const {
        if (index < coeffs.size())
            return coeffs[index];
        static double zero = 0.0;
        return zero;
    }

    double operator()(double x) const {
        double result = 0.0, power = 1.0;
        for (size_t i = 0; i < coeffs.size(); i++) {
            result += coeffs[i] * power;
            power *= x;
        }
        return result;
    }

    unsigned degree() const {
        int d = coeffs.size() - 1;
        while (d > 0 && fabs(coeffs[d]) < 1e-9)
            d--;
        return d;
    }

    explicit operator bool() const {
        for (size_t i = 0; i < coeffs.size(); i++)
            if (fabs(coeffs[i]) > 1e-9)
                return true;
        return false;
    }
    bool operator!() const {
        return !static_cast<bool>(*this);
    }

    CPolynomial & operator*=(double scalar) {
        for (size_t i = 0; i < coeffs.size(); i++)
            coeffs[i] *= scalar;
        if (fabs(scalar) < 1e-9)
            coeffs.assign(1, 0.0);
        return *this;
    }

    CPolynomial & operator*=(const CPolynomial & other) {
        vector<double> result(coeffs.size() + other.coeffs.size() - 1, 0.0);
        for (size_t i = 0; i < coeffs.size(); i++)
            for (size_t j = 0; j < other.coeffs.size(); j++)
                result[i + j] += coeffs[i] * other.coeffs[j];
        while (result.size() > 1 && fabs(result.back()) < 1e-9)
            result.pop_back();
        coeffs = result;
        return *this;
    }

    static void setPolynomVar(const string & s) {
        polyVar = s;
    }
    static const string & getPolynomVar() {
        return polyVar;
    }

    friend CPolynomial operator*(const CPolynomial & poly, double scalar);
    friend CPolynomial operator*(double scalar, const CPolynomial & poly);
    friend CPolynomial operator*(const CPolynomial & a, const CPolynomial & b);
    friend bool operator==(const CPolynomial & a, const CPolynomial & b);
    friend bool operator!=(const CPolynomial & a, const CPolynomial & b);
    friend ostream & operator<<(ostream & os, const CPolynomial & poly);

private:
    vector<double> coeffs;
    inline static string polyVar = "x";
};

CPolynomial operator*(const CPolynomial & poly, double scalar) {
    CPolynomial res = poly;
    res *= scalar;
    return res;
}
CPolynomial operator*(double scalar, const CPolynomial & poly) {
    return poly * scalar;
}
CPolynomial operator*(const CPolynomial & a, const CPolynomial & b) {
    CPolynomial res = a;
    res *= b;
    return res;
}

bool operator==(const CPolynomial & a, const CPolynomial & b) {
    unsigned da = a.degree(), db = b.degree();
    if (da != db)
        return false;
    for (size_t i = 0; i <= da; i++)
        if (fabs(a.coeffs[i] - b.coeffs[i]) > 1e-6)
            return false;
    return true;
}
bool operator!=(const CPolynomial & a, const CPolynomial & b) {
    return !(a == b);
}

ostream & operator<<(ostream & os, const CPolynomial & poly) {
    int deg = poly.degree();
    bool firstTerm = true;
    ostringstream oss;
    for (int i = deg; i >= 0; i--) {
        double c = poly[i];
        if (fabs(c) < 1e-9)
            continue;
        if (firstTerm) {
            if (c < 0)
                oss << "- ";
        }
        else {
            if (c < 0)
                oss << " - ";
            else
                oss << " + ";
        }
        double absC = fabs(c);
        if (i == 0)
            oss << absC;
        else {
            if (fabs(absC - 1.0) > 1e-9)
                oss << absC << "*";
            oss << CPolynomial::getPolynomVar() << "^" << i;
        }
        firstTerm = false;
    }
    if (firstTerm)
        oss << "0";
    os << oss.str();
    return os;
}


class PolynomVarManip {
public:
    PolynomVarManip(const string & name) : var(name){}
    friend ostream & operator<<(ostream & os, const PolynomVarManip & p) {
        CPolynomial::setPolynomVar(p.var);
        return os;
    }
private:
    string var;
};

PolynomVarManip poly_var(const string & name) {
    return PolynomVarManip(name);
}
