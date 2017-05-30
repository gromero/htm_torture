probe kernel.function("vsx_unavailable_tm").call {
  if (execname() == "torture") {
    printf("vsx_unavailable_tm.call:   cpuid: %ld tid: %ld, load_fp=%p, load_vec=%p, trap=%p, msr=%p, FP=%d, VMX=%d, VSX=%d, MSR_TS=%d, MSR_TM=%d, ck_msr=%p, ck_FP=%d, ck_VMX=%d, ck_VSX=%d ck_MSR_TS=%d. ck_MSR_TM=%d, ",
            cpu(), ns_tid(), task_current()->thread->load_fp, task_current()->thread->load_vec, $regs->trap, register("msr"),
                                                                                                             register("msr") & 1 << 13 ? 1 : 0, 
                                                                                                             register("msr") & 1 << 25 ? 1 : 0,
                                                                                                             register("msr") & 1 << 23 ? 1 : 0,
                                                                                                             register("msr") & (1<<33|1<<34) ? (register("msr") & (1<<33|1<<34)) >> 33 : 0,
                                                                                                             register("msr") & 1<<32 ? 1 : 0,
                                                                                                             task_current()->thread->ckpt_regs->msr,                   // MSR
                                                                                                             task_current()->thread->ckpt_regs->msr & 1 << 13 ? 1 : 0, // MSR_FP
                                                                                                             task_current()->thread->ckpt_regs->msr & 1 << 25 ? 1 : 0, // MSR_VMX
                                                                                                             task_current()->thread->ckpt_regs->msr & 1 << 23 ? 1 : 0, // MSR_VSX  
                                                                   task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS
                                                                   task_current()->thread->ckpt_regs->msr & 1 << 32 ? 1 : 0);                                          // MSR_TM
    print_regs();
  }
}
probe kernel.function("vsx_unavailable_tm").return {
  if (execname() == "torture") {
    printf("vsx_unavailable_tm.return: cpuid: %ld tid: %ld, load_fp=%p, load_vec=%p, trap=%p, msr=%p, FP=%d, VMX=%d, VSX=%d, MSR_TS=%d, MSR_TM=%d, ck_msr=%p, ck_FP=%d, ck_VMX=%d, ck_VSX=%d ck_MSR_TS=%d. ck_MSR_TM=%d, ",
            cpu(), ns_tid(), task_current()->thread->load_fp, task_current()->thread->load_vec, @entry($regs->trap), register("msr"),
                                                                                                             register("msr") & 1 << 13 ? 1 : 0, 
                                                                                                             register("msr") & 1 << 25 ? 1 : 0,
                                                                                                             register("msr") & 1 << 23 ? 1 : 0,
                                                                                                             register("msr") & (1<<33|1<<34) ? (register("msr") & (1<<33|1<<34)) >> 33 : 0,
                                                                                                             register("msr") & 1<<32 ? 1 : 0,
                                                                                                             task_current()->thread->ckpt_regs->msr,                   // MSR
                                                                                                             task_current()->thread->ckpt_regs->msr & 1 << 13 ? 1 : 0, // MSR_FP
                                                                                                             task_current()->thread->ckpt_regs->msr & 1 << 25 ? 1 : 0, // MSR_VMX
                                                                                                             task_current()->thread->ckpt_regs->msr & 1 << 23 ? 1 : 0, // MSR_VSX  
                                                                   task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS
                                                                   task_current()->thread->ckpt_regs->msr & 1 << 32 ? 1 : 0);                                          // MSR_TM
    print_regs();
  }
}


probe kernel.function("__switch_to").call {
  if (execname() == "torture") {
    printf("__switch_to.call:          cpuid: %ld tid: %ld, load_fp=%p, load_vec=%p, trap=%p, msr=%p, FP=%d, VMX=%d, VSX=%d, MSR_TS=%d, MSR_TM=%d, ck_msr=%p, ck_FP=%d, ck_VMX=%d, ck_VSX=%d ck_MSR_TS=%d. ck_MSR_TM=%d, ",
           cpu(), tid(), task_current()->thread->load_fp, task_current()->thread->load_vec, task_current()->thread->regs->trap, 
 													     register("msr"),
                                                                                                             register("msr") & 1 << 13 ? 1 : 0, 
                                                                                                             register("msr") & 1 << 25 ? 1 : 0,
                                                                                                             register("msr") & 1 << 23 ? 1 : 0,
                                                                                                             register("msr") & (1<<33|1<<34) ? (register("msr") & (1<<33|1<<34)) >> 33 : 0,
                                                                                                             register("msr") & 1<<32 ? 1 : 0,
                                                                                                             task_current()->thread->ckpt_regs->msr,                   // MSR
                                                                                                             task_current()->thread->ckpt_regs->msr & 1 << 13 ? 1 : 0, // MSR_FP
                                                                                                             task_current()->thread->ckpt_regs->msr & 1 << 25 ? 1 : 0, // MSR_VMX
                                                                                                             task_current()->thread->ckpt_regs->msr & 1 << 23 ? 1 : 0, // MSR_VSX  
                                                                   task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS
                                                                   task_current()->thread->ckpt_regs->msr & 1 << 32 ? 1 : 0);                                          // MSR_TM
    print_regs();
  }
}
probe kernel.function("__switch_to").return {
  if (execname() == "torture") {
    printf("__switch_to.return:        cpuid: %ld tid: %ld, load_fp=%p, load_vec=%p, trap=%p, msr=%p, FP=%d, VMX=%d, VSX=%d, MSR_TS=%d, MSR_TM=%d, ck_msr=%p, ck_FP=%d, ck_VMX=%d, ck_VSX=%d ck_MSR_TS=%d. ck_MSR_TM=%d, ",
           cpu(), tid(), task_current()->thread->load_fp, task_current()->thread->load_vec, task_current()->thread->regs->trap, 
 													     register("msr"),
                                                                                                             register("msr") & 1 << 13 ? 1 : 0, 
                                                                                                             register("msr") & 1 << 25 ? 1 : 0,
                                                                                                             register("msr") & 1 << 23 ? 1 : 0,
                                                                                                             register("msr") & (1<<33|1<<34) ? (register("msr") & (1<<33|1<<34)) >> 33 : 0,
                                                                                                             register("msr") & 1<<32 ? 1 : 0,
                                                                                                             task_current()->thread->ckpt_regs->msr,                   // MSR
                                                                                                             task_current()->thread->ckpt_regs->msr & 1 << 13 ? 1 : 0, // MSR_FP
                                                                                                             task_current()->thread->ckpt_regs->msr & 1 << 25 ? 1 : 0, // MSR_VMX
                                                                                                             task_current()->thread->ckpt_regs->msr & 1 << 23 ? 1 : 0, // MSR_VSX  
                                                                   task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS
                                                                   task_current()->thread->ckpt_regs->msr & 1 << 32 ? 1 : 0);                                          // MSR_TM
    print_regs();
  }
}

probe kernel.function("restore_fp").inline {
  if (execname() == "torture") {
    printf("restore_fp.call:          cpuid: %ld tid: %ld, load_fp=%p, load_vec=%p, trap=%p, msr=%p, FP=%d, VMX=%d, VSX=%d, MSR_TS=%d, MSR_TM=%d, ck_msr=%p, ck_FP=%d, ck_VMX=%d, ck_VSX=%d ck_MSR_TS=%d. ck_MSR_TM=%d, ",
           cpu(), tid(), task_current()->thread->load_fp, task_current()->thread->load_vec, task_current()->thread->regs->trap, 
 													     register("msr"),
                                                                                                             register("msr") & 1 << 13 ? 1 : 0, 
                                                                                                             register("msr") & 1 << 25 ? 1 : 0,
                                                                                                             register("msr") & 1 << 23 ? 1 : 0,
                                                                                                             register("msr") & (1<<33|1<<34) ? (register("msr") & (1<<33|1<<34)) >> 33 : 0,
                                                                                                             register("msr") & 1<<32 ? 1 : 0,
                                                                                                             task_current()->thread->ckpt_regs->msr,                   // MSR
                                                                                                             task_current()->thread->ckpt_regs->msr & 1 << 13 ? 1 : 0, // MSR_FP
                                                                                                             task_current()->thread->ckpt_regs->msr & 1 << 25 ? 1 : 0, // MSR_VMX
                                                                                                             task_current()->thread->ckpt_regs->msr & 1 << 23 ? 1 : 0, // MSR_VSX  
                                                                   task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS
                                                                   task_current()->thread->ckpt_regs->msr & 1 << 32 ? 1 : 0);                                          // MSR_TM
    print_regs();
  }
}
