#!/usr/bin/env bats

T_ACCEPTED_bin=./src/accepted
T_REJECTED_bin=./src/rejected
EXIT_SUCCESS=0
EXIT_FAILURE=1

@test "accepted: run no argument, exit code should be $EXIT_FAILURE" {
  run $T_ACCEPTED_bin
  [ "$status" -eq $EXIT_FAILURE ]
  [ "$output" == "" ]
}

@test "accepted: test the most siplistic case" {
  run $T_ACCEPTED_bin -y -Y y
  [ "$status" -eq $EXIT_SUCCESS ]
}


@test "accepted: banner version 1.0." {
  run $T_ACCEPTED_bin --version
  [ "$output" == "This is accepted 1.0." ]
}
