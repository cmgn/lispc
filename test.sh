#!/usr/bin/env bash

exitcode=0
for file in $(find testdata/ -regex 'testdata/[a-zA-Z0-9_]+\.lisp'); do
    expected="${file}.expected"
    received=$(./main "${file}")
    echo "${file}"
    if diff --color -B "${expected}" - <<<"${received}"; then
        echo "passed"
    else
        echo "failed"
        exitcode=1
    fi
done

if [[ "${exitcode}" -eq 0 ]]; then
    echo "all tests passed"
fi

exit "${exitcode}"