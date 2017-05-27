probe kernel.function("restore_math").return {
  if (execname() == "torture") {
    printf("\ntid: %ld, load_fp=%p, load_vec=%p, trap=%p, msr=%p\n",
            ns_tid(), task_current()->thread->load_fp, task_current()->thread->load_vec, $regs->trap, register("msr"));
//    if ($regs->trap == 0x700)
     print_stack(callers(-1));
  }
}
