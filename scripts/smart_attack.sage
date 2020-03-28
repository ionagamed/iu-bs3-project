# flake8: noqa

from argparse import ArgumentParser
import sys

from sage.all import *


def HenselLift(P, p, prec):
    E = P.curve()
    Eq = E.change_ring(QQ)
    Ep = Eq.change_ring(Qp(p,prec))
    x_P,y_P = P.xy()
    x_lift = ZZ(x_P)
    y_lift = ZZ(y_P)
    x, y, a1, a2, a3, a4, a6 = var('x,y,a1,a2,a3,a4,a6')
    f(a1,a2,a3,a4,a6,x,y) = y^2 + a1*x*y + a3*y - x^3 - a2*x^2 - a4*x - a6
    g(y) = f(ZZ(Eq.a1()),ZZ(Eq.a2()),ZZ(Eq.a3()),ZZ(Eq.a4()),ZZ(Eq.a6()),ZZ(x_P),y)
    gDiff = g.diff()
    for i in range(1,prec):
        uInv = ZZ(gDiff(y=y_lift))
        u = uInv.inverse_mod(p^i)
        y_lift = y_lift - u*g(y_lift)
        y_lift = ZZ(Mod(y_lift,p^(i+1)))
    y_lift = y_lift+O(p^prec)
    return Ep([x_lift,y_lift])


def SmartAttack(P, Q, p, prec):
    E = P.curve()
    Eqq = E.change_ring(QQ)
    Eqp = Eqq.change_ring(Qp(p, prec))

    P_Qp = HenselLift(P, p, prec)
    Q_Qp = HenselLift(Q, p, prec)

    p_times_P = p*P_Qp
    p_times_Q=p*Q_Qp

    x_P,y_P = p_times_P.xy()
    x_Q,y_Q = p_times_Q.xy()

    phi_P = -(x_P/y_P)
    phi_Q = -(x_Q/y_Q)

    k = phi_Q / phi_P
    k = Mod(k, p)
    return k


def parse_args():
    parser = ArgumentParser()
    # solving k * (px, py) = (gx, gy) for curve y^2 + a1.xy + a3.y = x^3 + a2.x^2 + a4.x + a6
    parser.add_argument('p', type=int)
    parser.add_argument('a1', type=int)
    parser.add_argument('a2', type=int)
    parser.add_argument('a3', type=int)
    parser.add_argument('a4', type=int)
    parser.add_argument('a6', type=int)
    parser.add_argument('px', type=int)
    parser.add_argument('py', type=int)
    parser.add_argument('qx', type=int)
    parser.add_argument('qy', type=int)
    return parser.parse_args(sys.argv[1:])


def main():
    args = parse_args()

    E = EllipticCurve(GF(args.p), [args.a1, args.a2, args.a3, args.a4, args.a6])
    P = E([args.px, args.py])
    Q = E([args.qx, args.qy])

    k = SmartAttack(P, Q, args.p, 9)
    print(k)


if __name__ == "__main__":
    main()
