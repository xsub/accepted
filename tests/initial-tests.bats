#!/usr/bin/env bats

ACCEPTED_bin=./src/accepted
REJECTED_bin=./src/rejected
EXIT_SUCCESS=0
EXIT_FAILURE=1

@test "accepted: no arguments exit code should be $EXIT_FAILURE" {
  run $ACCEPTED_bin
  [ "$status" -eq $EXIT_FAILURE ]
  [ "$output" == "" ]
}

@test "rejected: no arguments, exit code should be $EXIT_SUCCESS" {
  run $REJECTED_bin
  [ "$status" -eq $EXIT_SUCCESS ]
  [ "$output" == "" ]
}


@test "accepted: test the most simplistic case" {
  run $ACCEPTED_bin -y -Y y
  [ "$status" -eq $EXIT_SUCCESS ]
}


@test "accepted: --std-yes without input, should EXIT_FAILURE" {
  run $ACCEPTED_bin --std-yes
  [ "$status" -eq $EXIT_FAILURE ]
}

@test "rejected: --std-no without input, should EXIT_SUCCESS" {
  run $REJECTED_bin --std-no
  [ "$status" -eq $EXIT_SUCCESS ]
}

@test "accepted: --std-yes with proper user input: 'yeah'" {
  run $ACCEPTED_bin --std-yes yeah
  [ "$status" -eq $EXIT_SUCCESS ]
}

@test "accepted: banner version." {
  run $ACCEPTED_bin --version
  [ "$output" == "This is Accepted 0.1." ]
}

@test "accepted: help" {
  #run $ACCEPTED_bin --help
  #echo $result | /bin/grep Usage || true 
  $ACCEPTED_bin --help | grep Usage
}
