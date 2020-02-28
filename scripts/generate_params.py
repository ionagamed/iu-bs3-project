from argparse import ArgumentParser
import base64
import sys

from pyasn1.codec.der.encoder import encode
from pyasn1.codec.der.decoder import decode


def parse_args():
    parser = ArgumentParser()
    return parser.parse_args(sys.argv[1:])


def load_substrate():
    with open('../files/sec_params.pem', 'rb') as f:
        source = base64.b64decode(b''.join(f.read().split(b'\n')[1:-1]))
        obj, rest = decode(source)

        for field in obj:
            print(f'{field}: {obj[field]}')

    return obj


def modify_substrate(obj):
    generator = (
        '04'
        'DB4FF10EC057E9AE26B07D0280B7F4341DA5D1B1EAE06C7D'
        '9B2F2F6D9C5628A7844163D015BE86344082AA88D95E2F9D'
    )

    generator = (
        '04'
        '000000000000000000000000000000000000000000000000'
        '000000000000000000000000000000000000000000000002'
    )

    obj['field-1']['field-1'] = 2**192 - 2**32 - 2**12 - 2**8 - 2**7 - 2**6 - 2**3 - 1
    obj['field-2'][1] = bytes(b'\x04')
    obj['field-3'] = bytes.fromhex(generator)
    obj['field-4'] = int.from_bytes(
        bytes.fromhex('FFFFFFFFFFFFFFFFFFFFFFFE26F2FC170F69466A74DEFD8D'),
        byteorder='big'
    )
    obj['field-5'] = 1

    return obj


def write_back(obj):
    with open('../files/custom_params.pem', 'wb') as f:
        source = base64.b64encode(encode(obj))
        sep_source = b''
        for i in range(0, len(source), 64):
            sep_source += source[i:i + 64] + b'\n'
        f.write(b'-----BEGIN EC PARAMETERS-----\n')
        f.write(sep_source)
        f.write(b'-----END EC PARAMETERS-----')


def main():
    obj = load_substrate()
    obj = modify_substrate(obj)
    write_back(obj)


if __name__ == "__main__":
    main()
