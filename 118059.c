int main() {
	thisplay ("ldmia r1!, {r3, r4, r5}");
	thisplay ("stmia r1!, {r3, r4, r5}");
	thisplay ("bkpt 12");
return 0;
	thisplay("sub r1, r2, 0");
	thisplay("sub r1, r2, 4");
	thisplay("sub r1, r2, 5");
	thisplay("sub r1, r2, 7");
	thisplay("sub r3, 44");
return 0;
#if 0
	thisplay("mov r0, 11");
	thisplay("mov r0, r2");
	thisplay("mov r1, r4");
	thisplay("cmp r1, r2");
	thisplay("cmp r3, 44");
	thisplay("nop");
	thisplay("svc 15");
	thisplay("add r1, r2");
	thisplay("add r3, 44");
	thisplay("sub r1, r2, 3");
	thisplay("sub r3, 44");
	thisplay("tst r3,r4");
	thisplay("bx r3");
	thisplay("b 33");
	thisplay("b 0");
	thisplay("bne 44");
	thisplay("and r2,r3");
#endif
	// INVALID thisplay("ldr r1, [pc, r2]");
	// INVALID thisplay("ldr r1, [sp, r2]");
#if 0
	thisplay("ldr r1, [pc, 12]");
	thisplay("ldr r1, [sp, 24]");
	thisplay("ldr r1, [r2, r3]");
#endif
	// INVALID thisplay("str r1, [pc, 22]");
	// INVALID thisplay("str r1, [pc, r2]");
	// INVALID thisplay("str r1, [sp, r2]");
#if 0
   0:   8991            ldrh    r1, [r2, #12]
   2:   7b11            ldrb    r1, [r2, #12]
   4:   8191            strh    r1, [r2, #12]
   6:   7311            strb    r1, [r2, #12]
#endif
	thisplay("ldrh r1, [r2, 8]"); // aligned to 4
	thisplay("ldrh r1, [r3, 8]"); // aligned to 4
	thisplay("ldrh r1, [r4, 16]"); // aligned to 4
	thisplay("ldrh r1, [r2, 32]"); // aligned to 4
	thisplay("ldrb r1, [r2, 20]"); // aligned to 4
	thisplay("strh r1, [r2, 20]"); // aligned to 4
	thisplay("strb r1, [r2, 20]"); // aligned to 4
	thisplay("str r1, [sp, 20]"); // aligned to 4
	thisplay("str r1, [r2, 12]"); // OK
	thisplay("str r1, [r2, r3]");
return 0;
#if 0
	display("mov r0, 33");
	display("mov r1, 33");
	display("movne r0, 33");
	display("tst r0, r1, lsl #2");
	display("svc 0x80");
	display("sub r3, r1, r2");
	display("add r0, r1, r2");
	display("mov fp, 0");
	display("pop {pc}");
	display("pop {r3}");
	display("bx r1");
	display("bx r3");
	display("bx pc");
	display("blx fp");
	display("pop {pc}");
	display("add lr, pc, lr");
	display("adds r3, #8");
	display("adds r3, r2, #8");
	display("subs r2, #1");
	display("cmp r0, r4");
	display("cmp r7, pc");
	display("cmp r1, r3");
	display("mov pc, 44");
	display("mov pc, r3");
	display("push {pc}");
	display("pop {pc}");
	display("nop");
	display("ldr r1, [r2, 33]");
	display("ldr r1, [r2, r3]");
	display("ldr r3, [r4, r6]");
	display("str r1, [pc, 33]");
	display("str r1, [pc], 2");
	display("str r1, [pc, 3]");
	display("str r1, [pc, r4]");
	display("bx r3");
	display("bcc 33");
	display("blx r3");
	display("bne 0x1200");
	display("str r0, [r1]");
	display("push {fp,lr}");
	display("pop {fp,lr}");
	display("pop {pc}");
#endif

   //10ab4:       00047e30        andeq   r7, r4, r0, lsr lr
   //10ab8:       00036e70        andeq   r6, r3, r0, ror lr

	display("andeq r7, r4, r0, lsr lr");
	display("andeq r6, r3, r0, ror lr");
//  c4:   e8bd80f0        pop     {r4, r5, r6, r7, pc}
	display("pop {r4,r5,r6,r7,pc}");


#if 0
	display("blx r1");
	display("blx 0x8048");
#endif

#if 0
	display("b 0x123");
	display("bl 0x123");
	display("blt 0x123"); // XXX: not supported
#endif
	return 0;
}