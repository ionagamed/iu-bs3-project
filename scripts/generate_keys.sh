#!/bin/bash

center() {
  termwidth=80
  padding="$(printf '%0.1s' ={1..500})"
  printf '%*.*s %s %*.*s\n' 0 "$(((termwidth-2-${#1})/2))" "$padding" "$1" 0 "$(((termwidth-1-${#1})/2))" "$padding"
}

cd ../files/

center "Generating the origin curve"
openssl ecparam -name secp192k1 -out sec_params.pem -param_enc explicit

center "Generating the custom curve"
pip3 install pyasn1
cd ../scripts/
python3 generate_params.py
cd ../files/

center "Validating the params"
openssl ecparam -in custom_params.pem -text -noout

center "Generating the key, and validating it"
openssl ecparam -in custom_params.pem -genkey -out custom_key.pem
openssl ec -in custom_key.pem -text -noout
