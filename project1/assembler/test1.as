        lw      0       1       ten     # load reg1 with 10 (symbolic address)
        lw      0       2       neg1    # load reg2 with -1 (numeric address)
        lw      0       4       stAddr  # load reg4 with the address of 'start'
start   add     1       2       1       # decrement reg1
        beq     1       0       2       # goto end of program when reg1 == 0
        jalr    4       3               # go back to the beginning of the loop
        noop
done    halt                            # end of program
neg1    .fill   -1
ten     .fill   10
stAddr  .fill   start                   # will contain the address of 'start'
