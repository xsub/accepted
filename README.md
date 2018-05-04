

Brief
---
**This is `accepted` -- tiny console program to aid shell scripts in probing user confirmations.**


License
---
This software is licensed under MIT license.


Overview
---
This program comes as one binary `accepted` buts gets hardlinked to another instance called `rejected` on installation.

Both program take list of strings (forming the list of allowed answers) and string user already entered on prompt as answer.

Program compares the user input string against the lists of accepting and rejecting answers and returns respective exit code to the calling shell. Flags are intended to modify the profile of operation.

Behaviour changes depending on binary name making `accepted` wait for postive and `rejected` for negative answers (strings representing accepting and rejecting answers, repsectively).


Example usage (in Bash-like script)
----

```
bash$ read "Do you want to do_it (answer 'yes' to confirm)? " answer
bash$ [ accepted +yes $answer] && do_it
```

More
----
As default:

*`accepted`* expects `--std-yes` (option defined) strings or strings added with `+` prefix as arguments, if user input matches any of them the program exit code is `ACCEPTED` (0), othwerise exits with `REJECTED` (1).  

*`rejected`* expects `--std-no` (option defined) strings or strings added with `-` as arguments to return `REJECTED`, otherwise it returns `ACCEPTED` (inverted logic of the prompts).

> Note:
The example exit codes (in curly brackets above) are for POSIX-compatible systems.

There is a way to define case insensitive strings as parts of answer list by using `IC` prefix and quotation marks, i.e.: `+IC"<string>"`  or `-IC"<string>`.

> Example:

```
bash$ accepted -IC"no, I *DO NOT* want to do it"   
```

This makes the string 'no, i *do not* want to do it' the valid negating answer, disregarding it's case.

Compilation
----

```
autoreconf --install
./configure
make
```

> Note: See ./scripts/compile.sh for complete example; 

TODO
----

- [x] Initial code that does something and minimal development ecosystem.

- [ ] Add flags *--std-yes*, *--std-no*, *--stdyes*, *--stdno*

  - a) *--std-yes = +IC"y" +IC"yes" +IC"yep" +IC"yeah"* 
  - b) *--std-no  = -IC"n" -IC"no" -IC"nah" -IC"nope"*
 

- [ ] Solo-operation mode (doing actual prompt in accepted/rejected code).

> Example 1.
```
bash$ [ accepted --S --strict --stdmin "Do you want to" ] && do_it
```

renders to console: 

```
Do you want to (y/n)? 
```

and accept 'y', 'Y', 'n', 'N' as valid input.


> Example 2.

```	
bash$ [ accepted --S --strict --stdmax "Do you want to" ] && do_it
```

renders to console: 

```
Do you want to (y/n/yes/no/yup/nah/yeah/nope)?
``` 

and accept any of showed answers, ignoring case.

- [ ] MUCH MORE (well maybe not THAT much xD )

Options
---

```
--S: 		solo-operation: no need to do 'read' and pass the answer, additionally the passed
		prompt is printed with all variants of answers.

--strict:	this makes the input prompt accept only listed strings.
```

---
### (c) 2018 Pawel.Suchanecki@gmail.com (xsub)
---

