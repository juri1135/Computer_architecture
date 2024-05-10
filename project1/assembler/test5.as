        lw      0       1       ten     # load reg1 with 10 (symbolic address)
        lw      1       2       neg2    # load reg2 with -2 (symbolic address)
main    add     1       ten       1     # add reg1 and ten
        beq     0       1       2       # goto end of program when reg1==0
        bne     0       0       main    # go back to the beginning of the loop
        noop 
end     halt                            # end of program
ten     .fill   10 
neg2    .fill   -2 
stAddr  .fill   main                    # will contain the address of main (2)
