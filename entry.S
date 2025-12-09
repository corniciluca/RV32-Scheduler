.global trap_entry
.global switch_to_first_task
.align 4

.equ REGBYTES, 4     
.equ FRAME_SIZE, 144

switch_to_first_task:
    j restore_context

trap_entry:

    addi sp,sp,-FRAME_SIZE

    sw x1,  1*REGBYTES(sp)  # Return Address (ra)
    sw x3,  3*REGBYTES(sp)  # Global Pointer (gp)
    sw x4,  4*REGBYTES(sp)  # Thread Pointer (tp)
    sw x5,  5*REGBYTES(sp)  # t0
    sw x6,  6*REGBYTES(sp)  # t1
    sw x7,  7*REGBYTES(sp)  # t2
    sw x8,  8*REGBYTES(sp)  # s0/fp
    sw x9,  9*REGBYTES(sp)  # s1
    sw x10, 10*REGBYTES(sp) # a0
    sw x11, 11*REGBYTES(sp) # a1
    sw x12, 12*REGBYTES(sp) # a2
    sw x13, 13*REGBYTES(sp) # a3
    sw x14, 14*REGBYTES(sp) # a4
    sw x15, 15*REGBYTES(sp) # a5
    sw x16, 16*REGBYTES(sp) # a6
    sw x17, 17*REGBYTES(sp) # a7
    sw x18, 18*REGBYTES(sp) # s2
    sw x19, 19*REGBYTES(sp) # s3
    sw x20, 20*REGBYTES(sp) # s4
    sw x21, 21*REGBYTES(sp) # s5
    sw x22, 22*REGBYTES(sp) # s6
    sw x23, 23*REGBYTES(sp) # s7
    sw x24, 24*REGBYTES(sp) # s8
    sw x25, 25*REGBYTES(sp) # s9
    sw x26, 26*REGBYTES(sp) # s10
    sw x27, 27*REGBYTES(sp) # s11
    sw x28, 28*REGBYTES(sp) # t3
    sw x29, 29*REGBYTES(sp) # t4
    sw x30, 30*REGBYTES(sp) # t5
    sw x31, 31*REGBYTES(sp) # t6

    csrr t0, mepc
    csrr t1, mstatus
    sw t0, 32*REGBYTES(sp)  #Save mpec
    sw t1, 33*REGBYTES(sp)  #Save mstatus


    #current_task->sp = sp;
    la t0, current_task     
    lw t1, 0(t0)            
    sw sp, 0(t1)            

    call trap_handler

    j restore_context


restore_context:
    # sp = current_task->sp;
    la t0, current_task     
    lw t1, 0(t0)            
    lw sp, 0(t1)            

    lw t1, 33*REGBYTES(sp)  # Load mstatus
    lw t0, 32*REGBYTES(sp)  # Load mepc
    csrw mstatus, t1        
    csrw mepc, t0           

    lw x1,  1*REGBYTES(sp)  # Return Address (ra)
    lw x3,  3*REGBYTES(sp)  # Global Pointer (gp)
    lw x4,  4*REGBYTES(sp)  # Thread Pointer (tp)
    lw x5,  5*REGBYTES(sp)
    lw x6,  6*REGBYTES(sp)  # t1
    lw x7,  7*REGBYTES(sp)  # t2
    lw x8,  8*REGBYTES(sp)  # s0/fp
    lw x9,  9*REGBYTES(sp)  # s1
    lw x10, 10*REGBYTES(sp) # a0
    lw x11, 11*REGBYTES(sp) # a1
    lw x12, 12*REGBYTES(sp) # a2
    lw x13, 13*REGBYTES(sp) # a3
    lw x14, 14*REGBYTES(sp) # a4
    lw x15, 15*REGBYTES(sp) # a5
    lw x16, 16*REGBYTES(sp) # a6
    lw x17, 17*REGBYTES(sp) # a7
    lw x18, 18*REGBYTES(sp) # s2
    lw x19, 19*REGBYTES(sp) # s3
    lw x20, 20*REGBYTES(sp) # s4
    lw x21, 21*REGBYTES(sp) # s5
    lw x22, 22*REGBYTES(sp) # s6
    lw x23, 23*REGBYTES(sp) # s7
    lw x24, 24*REGBYTES(sp) # s8
    lw x25, 25*REGBYTES(sp) # s9
    lw x26, 26*REGBYTES(sp) # s10
    lw x27, 27*REGBYTES(sp) # s11
    lw x28, 28*REGBYTES(sp) # t3
    lw x29, 29*REGBYTES(sp) # t4
    lw x30, 30*REGBYTES(sp) # t5
    lw x31, 31*REGBYTES(sp) # t6

    addi sp, sp, FRAME_SIZE
    mret