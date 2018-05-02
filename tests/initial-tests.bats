#!/usr/bin/env bats

ACCEPTED_bin=./src/accepted
REJECTED_bin=./src/rejected
EXIT_SUCCESS=0
EXIT_FAILURE=1

@test "accepted: run no argument, exit code should be $EXIT_FAILURE" {
  run $ACCEPTED_bin
  [ "$status" -eq $EXIT_FAILURE ]
  [ "$output" == "" ]
}

@test "accepted: test the most siplistic case" {
  run $ACCEPTED_bin -y -Y y
  [ "$status" -eq $EXIT_SUCCESS ]
}


@test "accepted: --std-yes" {
  run $ACCEPTED_bin --std-yes yeah
  [ "$status" -eq $EXIT_SUCCESS ]
}

@test "accepted: banner version 1.0." {
  run $ACCEPTED_bin --version
  [ "$output" == "This is accepted 1.0." ]
}


@test "accepted: help" {
  #run $ACCEPTED_bin --help
  #echo $result | /bin/grep Usage || true 
  $ACCEPTED_bin --help | grep Usage
}
