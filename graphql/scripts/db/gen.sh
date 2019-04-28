#! /bin/bash

set -e
pushd $(/bin/pwd) > /dev/null

BASE="$(realpath $0)"
SCRIPTS_DIR="$(dirname $BASE)"
BASE_DIR="$SCRIPTS_DIR/../.."

sqlboiler \
    -c "$SCRIPTS_DIR/sqlboiler.toml" \
    -o "$BASE_DIR/pkg/models" \
    mysql

popd > /dev/null
