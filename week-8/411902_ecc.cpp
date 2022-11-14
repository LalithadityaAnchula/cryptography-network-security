#include <cstdlib>
#include <iostream>
#include <vector>
using namespace std;

#include <math.h>
#include "FiniteFieldElement.hpp"

namespace Cryptography
{
    template <int P>
    class EllipticCurve
    {
    public:
        typedef FiniteFieldElement<P> ffe_t;
        class Point
        {
            friend class EllipticCurve<P>;
            typedef FiniteFieldElement<P> ffe_t;
            ffe_t x_;
            ffe_t y_;
            EllipticCurve *ec_;

            void addDouble(int m, Point &acc)
            {
                if (m > 0)
                {
                    Point r = acc;
                    for (int n = 0; n < m; ++n)
                    {
                        r += r; // doubling step
                    }
                    acc = r;
                }
            }
            Point scalarMultiply(int k, const Point &a)
            {
                Point acc = a;
                Point res = Point(0, 0, *ec_);
                int i = 0, j = 0;
                int b = k;

                while (b)
                {
                    if (b & 1)
                    {
                        // bit is set; acc = 2^(i-j)*acc
                        addDouble(i - j, acc);
                        res += acc;
                        j = i; // last bit set
                    }
                    b >>= 1;
                    ++i;
                }
                return res;
            }

            void add(ffe_t x1, ffe_t y1, ffe_t x2, ffe_t y2, ffe_t &xR, ffe_t &yR) const
            {

                if (x1 == 0 && y1 == 0)
                {
                    xR = x2;
                    yR = y2;
                    return;
                }
                if (x2 == 0 && y2 == 0)
                {
                    xR = x1;
                    yR = y1;
                    return;
                }
                if (y1 == -y2)
                {
                    xR = yR = 0;
                    return;
                }

                // the additions
                ffe_t s;
                if (x1 == x2 && y1 == y2)
                {
                    // 2P
                    s = (3 * (x1.i() * x1.i()) + ec_->a()) / (2 * y1);
                    xR = ((s * s) - 2 * x1);
                }
                else
                {
                    // P+Q
                    s = (y1 - y2) / (x1 - x2);
                    xR = ((s * s) - x1 - x2);
                }

                if (s != 0)
                {
                    yR = (-y1 + s * (x1 - xR));
                }
                else
                {
                    xR = yR = 0;
                }
            }

            Point(int x, int y)
                : x_(x),
                  y_(y),
                  ec_(0)
            {
            }

            Point(int x, int y, EllipticCurve<P> &EllipticCurve)
                : x_(x),
                  y_(y),
                  ec_(&EllipticCurve)
            {
            }

            Point(const ffe_t &x, const ffe_t &y, EllipticCurve<P> &EllipticCurve)
                : x_(x),
                  y_(y),
                  ec_(&EllipticCurve)
            {
            }

        public:
            static Point ONE;

            // copy ctor
            Point(const Point &rhs)
            {
                x_ = rhs.x_;
                y_ = rhs.y_;
                ec_ = rhs.ec_;
            }
            // assignment
            Point &operator=(const Point &rhs)
            {
                x_ = rhs.x_;
                y_ = rhs.y_;
                ec_ = rhs.ec_;
                return *this;
            }
            // access x component as element of Fp
            ffe_t x() const { return x_; }
            // access y component as element of Fp
            ffe_t y() const { return y_; }
            unsigned int Order(unsigned int maxPeriod = ~0) const
            {
                Point r = *this;
                unsigned int n = 0;
                while (r.x_ != 0 && r.y_ != 0)
                {
                    ++n;
                    r += *this;
                    if (n > maxPeriod)
                        break;
                }
                return n;
            }
            // negate
            Point operator-()
            {
                return Point(x_, -y_);
            }
            // ==
            friend bool operator==(const Point &lhs, const Point &rhs)
            {
                return (lhs.ec_ == rhs.ec_) && (lhs.x_ == rhs.x_) && (lhs.y_ == rhs.y_);
            }
            // !=
            friend bool operator!=(const Point &lhs, const Point &rhs)
            {
                return (lhs.ec_ != rhs.ec_) || (lhs.x_ != rhs.x_) || (lhs.y_ != rhs.y_);
            }
            // a + b
            friend Point operator+(const Point &lhs, const Point &rhs)
            {
                ffe_t xR, yR;
                lhs.add(lhs.x_, lhs.y_, rhs.x_, rhs.y_, xR, yR);
                return Point(xR, yR, *lhs.ec_);
            }
            // a * int
            friend Point operator*(int k, const Point &rhs)
            {
                return Point(rhs).operator*=(k);
            }
            // +=
            Point &operator+=(const Point &rhs)
            {
                add(x_, y_, rhs.x_, rhs.y_, x_, y_);
                return *this;
            }
            // a *= int
            Point &operator*=(int k)
            {
                return (*this = scalarMultiply(k, *this));
            }
            // ostream handler: print this point
            friend ostream &operator<<(ostream &os, const Point &p)
            {
                return (os << "(" << p.x_ << ", " << p.y_ << ")");
            }
        };

        // ==================================================== EllipticCurve impl

        typedef EllipticCurve<P> this_t;
        typedef class EllipticCurve<P>::Point point_t;

        // ctor
        // Initialize EC as y^2 = x^3 + ax + b
        EllipticCurve(int a, int b)
            : a_(a),
              b_(b),
              m_table_(),
              table_filled_(false)
        {
        }

        void CalculatePoints()
        {
            int x_val[P];
            int y_val[P];
            for (int n = 0; n < P; ++n)
            {
                int nsq = n * n;
                x_val[n] = ((n * nsq) + a_.i() * n + b_.i()) % P;
                y_val[n] = nsq % P;
            }

            for (int n = 0; n < P; ++n)
            {
                for (int m = 0; m < P; ++m)
                {
                    if (x_val[n] == y_val[m])
                    {
                        m_table_.push_back(Point(n, m, *this));
                    }
                }
            }

            table_filled_ = true;
        }
        // get a point (group element) on the curve
        Point operator[](int n)
        {
            if (!table_filled_)
            {
                CalculatePoints();
            }

            return m_table_[n];
        }
        // number of elements in this group
        size_t Size() const { return m_table_.size(); }
        // the degree P of this EC
        int Degree() const { return P; }
        // the parameter a (as an element of Fp)
        FiniteFieldElement<P> a() const { return a_; }
        // the paramter b (as an element of Fp)
        FiniteFieldElement<P> b() const { return b_; }

        // ostream handler: print this curve in human readable form
        template <int T>
        friend ostream &operator<<(ostream &os, const EllipticCurve<T> &EllipticCurve);
        // print all the elements of the EC group
        ostream &PrintTable(ostream &os, int columns = 4);

    private:
        typedef std::vector<Point> m_table_t;

        m_table_t m_table_;       // table of points
        FiniteFieldElement<P> a_; // paramter a of the EC equation
        FiniteFieldElement<P> b_; // parameter b of the EC equation
        bool table_filled_;       // true if the table has been calculated
    };

    template <int T>
    typename EllipticCurve<T>::Point EllipticCurve<T>::Point::ONE(0, 0);

    template <int T>
    ostream &operator<<(ostream &os, const EllipticCurve<T> &EllipticCurve)
    {
        os << "y^2 mod " << T << " = (x^3" << showpos;
        if (EllipticCurve.a_ != 0)
        {
            os << EllipticCurve.a_ << "x";
        }

        if (EllipticCurve.b_.i() != 0)
        {
            os << EllipticCurve.b_;
        }

        os << noshowpos << ") mod " << T;
        return os;
    }

    template <int P>
    ostream &EllipticCurve<P>::PrintTable(ostream &os, int columns)
    {
        if (table_filled_)
        {
            int col = 0;
            typename EllipticCurve<P>::m_table_t::iterator iter = m_table_.begin();
            for (; iter != m_table_.end(); ++iter)
            {
                os << "(" << (*iter).x_.i() << ", " << (*iter).y_.i() << ") ";
                if (++col > columns)
                {
                    os << "\n";
                    col = 0;
                }
            }
        }
        else
        {
            os << "EllipticCurve, F_" << P;
        }
        return os;
    }
}

namespace utils
{
    float frand()
    {
        static float norm = 1.0f / (float)RAND_MAX;
        return (float)rand() * norm;
    }

    int irand(int min, int max)
    {
        return min + (int)(frand() * (float)(max - min));
    }
}

using namespace Cryptography;
using namespace utils;

int main(int argc, char *argv[])
{
    typedef EllipticCurve<263> ec_t;
    ec_t myEllipticCurve(1, 1);

    cout << "A little Elliptic Curve cryptography example\nby Jarl Ostensen, 2007\n\n";

    // print out a little info and test some properties
    cout << "The elliptic curve: " << myEllipticCurve << "\n";

    // calulate all the points for this curve. NOTE: in the real world this would not
    // be a very sensible thing to do. If the period is very large this is big and slow
    myEllipticCurve.CalculatePoints();

    cout << "\nPoints on the curve (i.e. the group elements):\n";
    myEllipticCurve.PrintTable(cout, 5);
    cout << "\n\n";

    ec_t::Point P = myEllipticCurve[2];
    cout << "some point P  = " << P << ", 2P = " << (P + P) << "\n";
    ec_t::Point Q = myEllipticCurve[3];
    cout << "some point Q = " << Q << ", P+Q = " << (P + Q) << "\n";
    ec_t::Point R = P;
    R += Q;
    cout << "P += Q = " << R << "\n";
    R = P;
    R += R;
    cout << "P += P = 2P = " << R << "\n";

    cout << "\nEC message encryption example\n===============================================\n\n";

    // Menes-Vanstone EC message encryption scheme

    // Public: the base point on the curve (i.e. base group element) used to generate keys
    // this is a REALLY slow way of picking a base point...
    ec_t::Point G = myEllipticCurve[0];
    while ((G.y() == 0 || G.x() == 0) || (G.Order() < 2))
    {
        int n = (int)(frand() * myEllipticCurve.Size());
        G = myEllipticCurve[n];
    }

    cout << "G = " << G << ", order(G) is " << G.Order() << "\n";

    // Alice
    int a = irand(1, myEllipticCurve.Degree() - 1);
    ec_t::Point Pa = a * G; // public key
    cout << "Alice' public key Pa = " << a << "*" << G << " = " << Pa << endl;

    // Bob
    int b = irand(1, myEllipticCurve.Degree() - 1);
    ;
    ec_t::Point Pb = b * G; // public key
    cout << "Bob's public key Pb = " << b << "*" << G << " = " << Pb << endl;

    // Jane, the eavesdropper
    int j = irand(1, myEllipticCurve.Degree() - 1);
    ;
    ec_t::Point Pj = j * G;
    cout << "Jane's public key Pj = " << j << "*" << G << " = " << Pj << endl;

    cout << "\n\n";

    // Alice encrypts her message to send to Bob
    // NOTE: the message first has to be split up into chunks that are in the Galois field F_p that is
    // the domain of the EC
    // With P quite small (like in these examples) this is a serious limitation, but in the real world
    // P could be very sizeable indeed, thus providing enough bits for good chunks
    int m1 = 19;
    int m2 = 72;

    cout << "Plain text message from Alice to Bob: (" << m1 << ", " << m2 << ")\n";

    // encrypt using Bob`s key
    ec_t::Point Pk = a * Pb;
    ec_t::ffe_t c1(m1 * Pk.x());
    ec_t::ffe_t c2(m2 * Pk.y());

    // encrypted message is: Pa,c1,c2
    cout << "Encrypted message from Alice to Bob = {Pa,c1,c2} = {" << Pa << ", " << c1 << ", " << c2 << "}\n\n";

    // Bob now decrypts Alice`s message, using her public key and his session integer "b" which was also used to generate his public key
    Pk = b * Pa;
    ec_t::ffe_t m1d = c1 / Pk.x();
    ec_t::ffe_t m2d = c2 / Pk.y();

    cout << "\tBob's decrypted message from Alice = (" << m1d << ", " << m2d << ")" << endl;

    Pk = j * Pa;
    m1d = c1 / Pk.x();
    m2d = c2 / Pk.y();

    cout << "\nJane's decrypted message from Alice = (" << m1d << ", " << m2d << ")" << endl;

    cout << endl;

    system("PAUSE");
    return EXIT_SUCCESS;
}
