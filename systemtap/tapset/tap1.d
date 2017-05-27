probe kernel.function("restore_math").return {
  if (execname() == "torture") {
    printf("tid: %ld, load_fp=%p, load_vec=%p, trap=%p\n", ns_tid(), task_current()->thread->load_fp, task_current()->thread->load_vec, $regs->trap);
  }
}
