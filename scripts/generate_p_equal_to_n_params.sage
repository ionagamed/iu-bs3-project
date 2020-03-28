"""
generate_p_equal_to_n_params.sage

Generates an elliptic curve that has an order equal to the prime field order.

Uses an approach outlined in http://www.monnerat.info/publications/anomalous.pdf.
"""

# flake8: noqa

from argparse import ArgumentParser
import sys


def parse_args():
    parser = ArgumentParser()
    return parser.parse_args(sys.argv[1:])


def main():
    args = parse_args()

    # this is only one example of such curve

    # generating (well, actually just settings) parameters
    D = 11
    m = 257743850762632419871495
    p = 11 * m * (m + 1) + 3
    j = GF(p)(-2^15)

    a = -3 * j / (j - 1728)
    b = 2 * j / (j - 1728)

    # validating that everything is ok
    E = EllipticCurve(GF(p), [a, b])
    P = E.random_element()

    # additionally create a random generator
    G = E.random_element()
    Gx, Gy = G.xy()

    if P.order() != p:
        print('Something is wrong')
    else:
        print(p)
        print(a)
        print(b)
        print(Gx)
        print(Gy)
        print(p)  # order is p, we know it


if __name__ == "__main__":
    main()
