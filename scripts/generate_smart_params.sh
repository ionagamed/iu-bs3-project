#!/bin/bash

center() {
  termwidth=80
  padding="$(printf '%0.1s' ={1..500})"
  printf '%*.*s %s %*.*s\n' 0 "$(((termwidth-2-${#1})/2))" "$padding" "$1" 0 "$(((termwidth-1-${#1})/2))" "$padding"
}

(cd ../c_tools/ && make)

cd ../files/

center "Generating the params and writing them into the file"
sage ../scripts/generate_p_equal_to_n_params.sage \
  | xargs -n 6 ../c_tools/bin/generate_curve_by_params smart_params.pem

center "Validating the params"
openssl ecparam -in smart_params.pem -text -noout

center "Generating a private key"
openssl ecparam -in smart_params.pem -genkey -out smart_key.pem

center "Validating the key"
openssl ec -in smart_key.pem -text -noout

center "Generating a public key"
openssl ec -in smart_key.pem -pubout -out smart_key.pub.pem

center "Validating the public key"
openssl ec -pubin -in smart_key.pub.pem -text -noout
