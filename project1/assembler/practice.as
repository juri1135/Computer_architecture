   lw   0   1   a
   lw   0   3   one
   nor   1   1   2
   add   2   3   2
   add    1   2   4   
   beq   0   4   z
   noop
z   halt
a   .fill   7
one   .fill   1
