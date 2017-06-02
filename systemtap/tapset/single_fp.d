probe kernel.function("__switch_to").call {
  if (execname() == "fp_torture") {
    printf("__switch_to.call           cpuid: %ld  tid: %ld "
                                                       ""
                                            "load_fp=%p "
                                           "load_vec=%p \n",

                                             cpu(), tid(),

                          task_current()->thread->load_fp,
                         task_current()->thread->load_vec);
  }
}
