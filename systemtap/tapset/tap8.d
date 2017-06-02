/*
probe kernel.function("vsx_unavailable_tm").call {
  if (execname() == "torture") {
    printf("vsx_unavailable_tm.call    cpuid: %ld  tid: %ld "
                                                       ""
                                            "load_fp=%p "
                                           "load_vec=%p "
                                            "used_vr=%p "
                                           "used_vsr=%p "
                                               "trap=%p "
                                                       ""
                                                "msr=%p "
                                                 "FP=%d "
                                                "VMX=%d "
                                                "VSX=%d "
                                             "MSR_TS=%d "
                                             "MSR_TM=%d "
                                                       ""
                                          "regs->msr=%p "
                                       "regs->msr_FP=%d "
                                      "regs->msr_VMX=%d "
                                      "regs->msr_VSX=%d "
                                   "regs->msr_MSR_TS=%d "
                                   "regs->msr_MSR_TM=%d "
                                    "vr_state->vr[0]=%p "
                                                       ""
                                     "ckpt_regs->msr=%p "
                                  "ckpt_regs->msr_FP=%d "
                                 "ckpt_regs->msr_VMX=%d "
                                 "ckpt_regs->msr_VSX=%d "
                              "ckpt_regs->msr_MSR_TS=%d "
                              "ckpt_regs->msr_MSR_TM=%d "
                                  "ckvr_state->vr[0]=%p",

                                            cpu(), tid(),

                          task_current()->thread->load_fp,
                         task_current()->thread->load_vec,
                          task_current()->thread->used_vr,
                         task_current()->thread->used_vsr,
                                              $regs->trap,

                                    // Read from register.
                                          register("msr"), // MSR
                        register("msr") & 1 << 13 ? 1 : 0, // MSR_FP
                        register("msr") & 1 << 25 ? 1 : 0, // MSR_VEC
                        register("msr") & 1 << 23 ? 1 : 0, // MSR_VSX
                        register("msr") & (1 << 33 |1 << 34) ? (register("msr") & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                          register("msr") & 1<<32 ? 1 : 0,

                                // Read from thread->regs.
                        task_current()->thread->regs->msr,  // MSR
      task_current()->thread->regs->msr & 1 << 13 ? 1 : 0,  // MSR_FP
      task_current()->thread->regs->msr & 1 << 25 ? 1 : 0,  // MSR_VEC
      task_current()->thread->regs->msr & 1 << 23 ? 1 : 0,  // MSR_VSX
    task_current()->thread->regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
      task_current()->thread->regs->msr & 1 << 32 ? 1 : 0,  // MSR_TM
      task_current()->thread->vr_state->vr, // vr_state->vr[0]

                           // Read from thread->ckpt_regs.
                   task_current()->thread->ckpt_regs->msr,  // MSR
 task_current()->thread->ckpt_regs->msr & 1 << 13 ? 1 : 0,  // MSR_FP
 task_current()->thread->ckpt_regs->msr & 1 << 25 ? 1 : 0,  // MSR_VEC
 task_current()->thread->ckpt_regs->msr & 1 << 23 ? 1 : 0,  // MSR_VSX
 task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
 task_current()->thread->ckpt_regs->msr & 1 << 32 ? 1 : 0, // MSR_TM
   task_current()->thread->ckvr_state->vr);

 // Print vsx32 (aka vmx0) using hackish version of print_regs().
 print_regs();
  }
}


probe kernel.function("vsx_unavailable_tm").return {
  if (execname() == "torture") {
    printf("vsx_unavailable_tm.return  cpuid: %ld  tid: %ld "
                                                       ""
                                            "load_fp=%p "
                                           "load_vec=%p "
                                            "used_vr=%p "
                                           "used_vsr=%p "
                                               "trap=%p "
                                                       ""
                                                "msr=%p "
                                                 "FP=%d "
                                                "VMX=%d "
                                                "VSX=%d "
                                             "MSR_TS=%d "
                                             "MSR_TM=%d "
                                                       ""
                                          "regs->msr=%p "
                                       "regs->msr_FP=%d "
                                      "regs->msr_VMX=%d "
                                      "regs->msr_VSX=%d "
                                   "regs->msr_MSR_TS=%d "
                                   "regs->msr_MSR_TM=%d "
                                    "vr_state->vr[0]=%p "
                                                       ""
                                     "ckpt_regs->msr=%p "
                                  "ckpt_regs->msr_FP=%d "
                                 "ckpt_regs->msr_VMX=%d "
                                 "ckpt_regs->msr_VSX=%d "
                              "ckpt_regs->msr_MSR_TS=%d "
                              "ckpt_regs->msr_MSR_TM=%d "
                                  "ckvr_state->vr[0]=%p",
                                             cpu(), tid(),

                          task_current()->thread->load_fp,
                         task_current()->thread->load_vec,
                          task_current()->thread->used_vr,
                         task_current()->thread->used_vsr,
                                      @entry($regs->trap),

                                    // Read from register.
                                          register("msr"), // MSR
                        register("msr") & 1 << 13 ? 1 : 0, // MSR_FP
                        register("msr") & 1 << 25 ? 1 : 0, // MSR_VEC
                        register("msr") & 1 << 23 ? 1 : 0, // MSR_VSX
                        register("msr") & (1 << 33 |1 << 34) ? (register("msr") & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                          register("msr") & 1<<32 ? 1 : 0,

                                // Read from thread->regs.
                        task_current()->thread->regs->msr,  // MSR
      task_current()->thread->regs->msr & 1 << 13 ? 1 : 0,  // MSR_FP
      task_current()->thread->regs->msr & 1 << 25 ? 1 : 0,  // MSR_VEC
      task_current()->thread->regs->msr & 1 << 23 ? 1 : 0,  // MSR_VSX
    task_current()->thread->regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
      task_current()->thread->regs->msr & 1 << 32 ? 1 : 0,  // MSR_TM
      task_current()->thread->vr_state->vr, // vr_state->vr[0]

                           // Read from thread->ckpt_regs.
                   task_current()->thread->ckpt_regs->msr,  // MSR
 task_current()->thread->ckpt_regs->msr & 1 << 13 ? 1 : 0,  // MSR_FP
 task_current()->thread->ckpt_regs->msr & 1 << 25 ? 1 : 0,  // MSR_VEC
 task_current()->thread->ckpt_regs->msr & 1 << 23 ? 1 : 0,  // MSR_VSX
 task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
 task_current()->thread->ckpt_regs->msr & 1 << 32 ? 1 : 0, // MSR_TM
   task_current()->thread->ckvr_state->vr);

 // Print vsx32 (aka vmx0) using hackish version of print_regs().
 print_regs();
  }
}
*/
probe kernel.function("__switch_to").call {
  if (execname() == "fp_torture") {
    printf("__switch_to.call           cpuid: %ld  tid: %ld "
                                                       ""
                                            "load_fp=%p "
                                           "load_vec=%p "
                                            "used_vr=%p "
                                           "used_vsr=%p "
                                               "trap=%p "
                                                       ""
                                                "msr=%p "
                                                 "FP=%d "
                                                "VMX=%d "
                                                "VSX=%d "
                                             "MSR_TS=%d "
                                             "MSR_TM=%d "
                                                       ""
                                          "regs->msr=%p "
                                       "regs->msr_FP=%d "
                                      "regs->msr_VMX=%d "
                                      "regs->msr_VSX=%d "
                                   "regs->msr_MSR_TS=%d "
                                   "regs->msr_MSR_TM=%d "
                                                       ""
                                     "ckpt_regs->msr=%p "
                                  "ckpt_regs->msr_FP=%d "
                                 "ckpt_regs->msr_VMX=%d "
                                 "ckpt_regs->msr_VSX=%d "
                              "ckpt_regs->msr_MSR_TS=%d "
                              "ckpt_regs->msr_MSR_TM=%d ",

                                             cpu(), tid(),

                          task_current()->thread->load_fp,
                         task_current()->thread->load_vec,
                          task_current()->thread->used_vr,
                         task_current()->thread->used_vsr,
                       task_current()->thread->regs->trap,

                                    // Read from register.
                                          register("msr"), // MSR
                        register("msr") & 1 << 13 ? 1 : 0, // MSR_FP
                        register("msr") & 1 << 25 ? 1 : 0, // MSR_VEC
                        register("msr") & 1 << 23 ? 1 : 0, // MSR_VSX
                        register("msr") & (1 << 33 |1 << 34) ? (register("msr") & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                          register("msr") & 1<<32 ? 1 : 0,

                                // Read from thread->regs.
                        task_current()->thread->regs->msr,  // MSR
      task_current()->thread->regs->msr & 1 << 13 ? 1 : 0,  // MSR_FP
      task_current()->thread->regs->msr & 1 << 25 ? 1 : 0,  // MSR_VEC
      task_current()->thread->regs->msr & 1 << 23 ? 1 : 0,  // MSR_VSX
    task_current()->thread->regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
      task_current()->thread->regs->msr & 1 << 32 ? 1 : 0,  // MSR_TM

                           // Read from thread->ckpt_regs.
                   task_current()->thread->ckpt_regs->msr,  // MSR
 task_current()->thread->ckpt_regs->msr & 1 << 13 ? 1 : 0,  // MSR_FP
 task_current()->thread->ckpt_regs->msr & 1 << 25 ? 1 : 0,  // MSR_VEC
 task_current()->thread->ckpt_regs->msr & 1 << 23 ? 1 : 0,  // MSR_VSX
 task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
 task_current()->thread->ckpt_regs->msr & 1 << 32 ? 1 : 0); // MSR_TM

 // Print vsx32 (aka vmx0) using hackish version of print_regs().
 print_regs();
  }
}
probe kernel.function("__switch_to").return {
  if (execname() == "torture") {
    printf("__switch_to.return         cpuid: %ld  tid: %ld "
                                                       ""
                                            "load_fp=%p "
                                           "load_vec=%p "
                                            "used_vr=%p "
                                           "used_vsr=%p "
                                               "trap=%p "
                                                       ""
                                                "msr=%p "
                                                 "FP=%d "
                                                "VMX=%d "
                                                "VSX=%d "
                                             "MSR_TS=%d "
                                             "MSR_TM=%d "
                                                       ""
                                          "regs->msr=%p "
                                       "regs->msr_FP=%d "
                                      "regs->msr_VMX=%d "
                                      "regs->msr_VSX=%d "
                                   "regs->msr_MSR_TS=%d "
                                   "regs->msr_MSR_TM=%d "
                                                       ""
                                     "ckpt_regs->msr=%p "
                                  "ckpt_regs->msr_FP=%d "
                                 "ckpt_regs->msr_VMX=%d "
                                 "ckpt_regs->msr_VSX=%d "
                              "ckpt_regs->msr_MSR_TS=%d "
                              "ckpt_regs->msr_MSR_TM=%d ",

                                             cpu(), tid(),

                          task_current()->thread->load_fp,
                         task_current()->thread->load_vec,
                          task_current()->thread->used_vr,
                         task_current()->thread->used_vsr,
                       task_current()->thread->regs->trap,

                                    // Read from register.
                                          register("msr"), // MSR
                        register("msr") & 1 << 13 ? 1 : 0, // MSR_FP
                        register("msr") & 1 << 25 ? 1 : 0, // MSR_VEC
                        register("msr") & 1 << 23 ? 1 : 0, // MSR_VSX
                        register("msr") & (1 << 33 |1 << 34) ? (register("msr") & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                          register("msr") & 1<<32 ? 1 : 0,

                                // Read from thread->regs.
                        task_current()->thread->regs->msr,  // MSR
      task_current()->thread->regs->msr & 1 << 13 ? 1 : 0,  // MSR_FP
      task_current()->thread->regs->msr & 1 << 25 ? 1 : 0,  // MSR_VEC
      task_current()->thread->regs->msr & 1 << 23 ? 1 : 0,  // MSR_VSX
    task_current()->thread->regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
      task_current()->thread->regs->msr & 1 << 32 ? 1 : 0,  // MSR_TM

                           // Read from thread->ckpt_regs.
                   task_current()->thread->ckpt_regs->msr,  // MSR
 task_current()->thread->ckpt_regs->msr & 1 << 13 ? 1 : 0,  // MSR_FP
 task_current()->thread->ckpt_regs->msr & 1 << 25 ? 1 : 0,  // MSR_VEC
 task_current()->thread->ckpt_regs->msr & 1 << 23 ? 1 : 0,  // MSR_VSX
 task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
 task_current()->thread->ckpt_regs->msr & 1 << 32 ? 1 : 0); // MSR_TM

 // Print vsx32 (aka vmx0) using hackish version of print_regs().
 print_regs();
  }
}
/*
probe kernel.function("restore_fp").inline {
  if (execname() == "torture") {
    printf("restore_fp.inline          cpuid: %ld  tid: %ld "
                                                       ""
                                            "load_fp=%p "
                                           "load_vec=%p "
                                            "used_vr=%p "
                                           "used_vsr=%p "
                                               "trap=%p "
                                                       ""
                                                "msr=%p "
                                                 "FP=%d "
                                                "VMX=%d "
                                                "VSX=%d "
                                             "MSR_TS=%d "
                                             "MSR_TM=%d "
                                                       ""
                                          "regs->msr=%p "
                                       "regs->msr_FP=%d "
                                      "regs->msr_VMX=%d "
                                      "regs->msr_VSX=%d "
                                   "regs->msr_MSR_TS=%d "
                                   "regs->msr_MSR_TM=%d "
                                                       ""
                                     "ckpt_regs->msr=%p "
                                  "ckpt_regs->msr_FP=%d "
                                 "ckpt_regs->msr_VMX=%d "
                                 "ckpt_regs->msr_VSX=%d "
                              "ckpt_regs->msr_MSR_TS=%d "
                              "ckpt_regs->msr_MSR_TM=%d ",

                                             cpu(), tid(),

                          task_current()->thread->load_fp,
                         task_current()->thread->load_vec,
                          task_current()->thread->used_vr,
                         task_current()->thread->used_vsr,
                       task_current()->thread->regs->trap,

                                    // Read from register.
                                          register("msr"), // MSR
                        register("msr") & 1 << 13 ? 1 : 0, // MSR_FP
                        register("msr") & 1 << 25 ? 1 : 0, // MSR_VEC
                        register("msr") & 1 << 23 ? 1 : 0, // MSR_VSX
                        register("msr") & (1 << 33 |1 << 34) ? (register("msr") & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                          register("msr") & 1<<32 ? 1 : 0,

                                // Read from thread->regs.
                        task_current()->thread->regs->msr,  // MSR
      task_current()->thread->regs->msr & 1 << 13 ? 1 : 0,  // MSR_FP
      task_current()->thread->regs->msr & 1 << 25 ? 1 : 0,  // MSR_VEC
      task_current()->thread->regs->msr & 1 << 23 ? 1 : 0,  // MSR_VSX
    task_current()->thread->regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
      task_current()->thread->regs->msr & 1 << 32 ? 1 : 0,  // MSR_TM

                           // Read from thread->ckpt_regs.
                   task_current()->thread->ckpt_regs->msr,  // MSR
 task_current()->thread->ckpt_regs->msr & 1 << 13 ? 1 : 0,  // MSR_FP
 task_current()->thread->ckpt_regs->msr & 1 << 25 ? 1 : 0,  // MSR_VEC
 task_current()->thread->ckpt_regs->msr & 1 << 23 ? 1 : 0,  // MSR_VSX
 task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
 task_current()->thread->ckpt_regs->msr & 1 << 32 ? 1 : 0); // MSR_TM

 // Print vsx32 (aka vmx0) using hackish version of print_regs().
 print_regs();
  }
}
*/
probe kernel.function("load_up_fpu").call {
  if (execname() == "fp_torture") {
    printf("load_up_fpu.call           cpuid: %ld  tid: %ld "
                                                       ""
                                            "load_fp=%p "
                                           "load_vec=%p "
                                            "used_vr=%p "
                                           "used_vsr=%p "
                                               "trap=%p "
                                                       ""
                                                "msr=%p "
                                                 "FP=%d "
                                                "VMX=%d "
                                                "VSX=%d "
                                             "MSR_TS=%d "
                                             "MSR_TM=%d "
                                                       ""
                                          "regs->msr=%p "
                                       "regs->msr_FP=%d "
                                      "regs->msr_VMX=%d "
                                      "regs->msr_VSX=%d "
                                   "regs->msr_MSR_TS=%d "
                                   "regs->msr_MSR_TM=%d "
                                    "vr_state->vr[0]=%p "
                                                       ""
                                     "ckpt_regs->msr=%p "
                                  "ckpt_regs->msr_FP=%d "
                                 "ckpt_regs->msr_VMX=%d "
                                 "ckpt_regs->msr_VSX=%d "
                              "ckpt_regs->msr_MSR_TS=%d "
                              "ckpt_regs->msr_MSR_TM=%d "
                                  "ckvr_state->vr[0]=%p",

                                            cpu(), tid(),

                          task_current()->thread->load_fp,
                         task_current()->thread->load_vec,
                          task_current()->thread->used_vr,
                         task_current()->thread->used_vsr,
                       task_current()->thread->regs->trap,

                                    // Read from register.
                                          register("msr"), // MSR
                        register("msr") & 1 << 13 ? 1 : 0, // MSR_FP
                        register("msr") & 1 << 25 ? 1 : 0, // MSR_VEC
                        register("msr") & 1 << 23 ? 1 : 0, // MSR_VSX
                        register("msr") & (1 << 33 |1 << 34) ? (register("msr") & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                          register("msr") & 1<<32 ? 1 : 0,

                                // Read from thread->regs.
                        task_current()->thread->regs->msr,  // MSR
      task_current()->thread->regs->msr & 1 << 13 ? 1 : 0,  // MSR_FP
      task_current()->thread->regs->msr & 1 << 25 ? 1 : 0,  // MSR_VEC
      task_current()->thread->regs->msr & 1 << 23 ? 1 : 0,  // MSR_VSX
      task_current()->thread->regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
      task_current()->thread->regs->msr & 1 << 32 ? 1 : 0,  // MSR_TM
      task_current()->thread->vr_state->vr, // vr_state->vr[0]

                           // Read from thread->ckpt_regs.
                   task_current()->thread->ckpt_regs->msr,  // MSR
 task_current()->thread->ckpt_regs->msr & 1 << 13 ? 1 : 0,  // MSR_FP
 task_current()->thread->ckpt_regs->msr & 1 << 25 ? 1 : 0,  // MSR_VEC
 task_current()->thread->ckpt_regs->msr & 1 << 23 ? 1 : 0,  // MSR_VSX
 task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
 task_current()->thread->ckpt_regs->msr & 1 << 32 ? 1 : 0, // MSR_TM
                  task_current()->thread->ckvr_state->vr);

 // Print vsx32 (aka vmx0) using hackish version of print_regs().
 print_regs();
  }
}
probe kernel.function("load_up_fpu").return {
  if (execname() == "fp_torture") {
    printf("load_up_fpu.return         cpuid: %ld  tid: %ld "
                                                       ""
                                            "load_fp=%p "
                                           "load_vec=%p "
                                            "used_vr=%p "
                                           "used_vsr=%p "
                                               "trap=%p "
                                                       ""
                                                "msr=%p "
                                                 "FP=%d "
                                                "VMX=%d "
                                                "VSX=%d "
                                             "MSR_TS=%d "
                                             "MSR_TM=%d "
                                                       ""
                                          "regs->msr=%p "
                                       "regs->msr_FP=%d "
                                      "regs->msr_VMX=%d "
                                      "regs->msr_VSX=%d "
                                   "regs->msr_MSR_TS=%d "
                                   "regs->msr_MSR_TM=%d "
                                    "vr_state->vr[0]=%p "
                                                       ""
                                     "ckpt_regs->msr=%p "
                                  "ckpt_regs->msr_FP=%d "
                                 "ckpt_regs->msr_VMX=%d "
                                 "ckpt_regs->msr_VSX=%d "
                              "ckpt_regs->msr_MSR_TS=%d "
                              "ckpt_regs->msr_MSR_TM=%d "
                                  "ckvr_state->vr[0]=%p",

                                            cpu(), tid(),

                          task_current()->thread->load_fp,
                         task_current()->thread->load_vec,
                          task_current()->thread->used_vr,
                         task_current()->thread->used_vsr,
                       task_current()->thread->regs->trap,

                                    // Read from register.
                                          register("msr"), // MSR
                        register("msr") & 1 << 13 ? 1 : 0, // MSR_FP
                        register("msr") & 1 << 25 ? 1 : 0, // MSR_VEC
                        register("msr") & 1 << 23 ? 1 : 0, // MSR_VSX
                        register("msr") & (1 << 33 |1 << 34) ? (register("msr") & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                          register("msr") & 1<<32 ? 1 : 0,

                                // Read from thread->regs.
                        task_current()->thread->regs->msr,  // MSR
      task_current()->thread->regs->msr & 1 << 13 ? 1 : 0,  // MSR_FP
      task_current()->thread->regs->msr & 1 << 25 ? 1 : 0,  // MSR_VEC
      task_current()->thread->regs->msr & 1 << 23 ? 1 : 0,  // MSR_VSX
      task_current()->thread->regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
      task_current()->thread->regs->msr & 1 << 32 ? 1 : 0,  // MSR_TM
      task_current()->thread->vr_state->vr, // vr_state->vr[0]

                           // Read from thread->ckpt_regs.
                   task_current()->thread->ckpt_regs->msr,  // MSR
 task_current()->thread->ckpt_regs->msr & 1 << 13 ? 1 : 0,  // MSR_FP
 task_current()->thread->ckpt_regs->msr & 1 << 25 ? 1 : 0,  // MSR_VEC
 task_current()->thread->ckpt_regs->msr & 1 << 23 ? 1 : 0,  // MSR_VSX
 task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
 task_current()->thread->ckpt_regs->msr & 1 << 32 ? 1 : 0, // MSR_TM
                  task_current()->thread->ckvr_state->vr);

 // Print vsx32 (aka vmx0) using hackish version of print_regs().
 print_regs();
  }
}

probe kernel.function("load_up_vsx").call {
  if (execname() == "fp_torture") {
    printf("load_up_vsx.call           cpuid: %ld  tid: %ld "
                                                       ""
                                            "load_fp=%p "
                                           "load_vec=%p "
                                            "used_vr=%p "
                                           "used_vsr=%p "
                                               "trap=%p "
                                                       ""
                                                "msr=%p "
                                                 "FP=%d "
                                                "VMX=%d "
                                                "VSX=%d "
                                             "MSR_TS=%d "
                                             "MSR_TM=%d "
                                                       ""
                                          "regs->msr=%p "
                                       "regs->msr_FP=%d "
                                      "regs->msr_VMX=%d "
                                      "regs->msr_VSX=%d "
                                   "regs->msr_MSR_TS=%d "
                                   "regs->msr_MSR_TM=%d "
                                    "vr_state->vr[0]=%p "
                                                       ""
                                     "ckpt_regs->msr=%p "
                                  "ckpt_regs->msr_FP=%d "
                                 "ckpt_regs->msr_VMX=%d "
                                 "ckpt_regs->msr_VSX=%d "
                              "ckpt_regs->msr_MSR_TS=%d "
                              "ckpt_regs->msr_MSR_TM=%d "
                                  "ckvr_state->vr[0]=%p",

                                            cpu(), tid(),

                          task_current()->thread->load_fp,
                         task_current()->thread->load_vec,
                          task_current()->thread->used_vr,
                         task_current()->thread->used_vsr,
                       task_current()->thread->regs->trap,

                                    // Read from register.
                                          register("msr"), // MSR
                        register("msr") & 1 << 13 ? 1 : 0, // MSR_FP
                        register("msr") & 1 << 25 ? 1 : 0, // MSR_VEC
                        register("msr") & 1 << 23 ? 1 : 0, // MSR_VSX
                        register("msr") & (1 << 33 |1 << 34) ? (register("msr") & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                          register("msr") & 1<<32 ? 1 : 0,

                                // Read from thread->regs.
                        task_current()->thread->regs->msr,  // MSR
      task_current()->thread->regs->msr & 1 << 13 ? 1 : 0,  // MSR_FP
      task_current()->thread->regs->msr & 1 << 25 ? 1 : 0,  // MSR_VEC
      task_current()->thread->regs->msr & 1 << 23 ? 1 : 0,  // MSR_VSX
      task_current()->thread->regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
      task_current()->thread->regs->msr & 1 << 32 ? 1 : 0,  // MSR_TM
      task_current()->thread->vr_state->vr, // vr_state->vr[0]

                           // Read from thread->ckpt_regs.
                   task_current()->thread->ckpt_regs->msr,  // MSR
 task_current()->thread->ckpt_regs->msr & 1 << 13 ? 1 : 0,  // MSR_FP
 task_current()->thread->ckpt_regs->msr & 1 << 25 ? 1 : 0,  // MSR_VEC
 task_current()->thread->ckpt_regs->msr & 1 << 23 ? 1 : 0,  // MSR_VSX
 task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
 task_current()->thread->ckpt_regs->msr & 1 << 32 ? 1 : 0, // MSR_TM
                  task_current()->thread->ckvr_state->vr);

 // Print vsx32 (aka vmx0) using hackish version of print_regs().
 print_regs();
  }
}
probe kernel.function("load_up_vsx").return {
  if (execname() == "fp_torture") {
    printf("load_up_vsx.return         cpuid: %ld  tid: %ld "
                                                       ""
                                            "load_fp=%p "
                                           "load_vec=%p "
                                            "used_vr=%p "
                                           "used_vsr=%p "
                                               "trap=%p "
                                                       ""
                                                "msr=%p "
                                                 "FP=%d "
                                                "VMX=%d "
                                                "VSX=%d "
                                             "MSR_TS=%d "
                                             "MSR_TM=%d "
                                                       ""
                                          "regs->msr=%p "
                                       "regs->msr_FP=%d "
                                      "regs->msr_VMX=%d "
                                      "regs->msr_VSX=%d "
                                   "regs->msr_MSR_TS=%d "
                                   "regs->msr_MSR_TM=%d "
                                    "vr_state->vr[0]=%p "
                                                       ""
                                     "ckpt_regs->msr=%p "
                                  "ckpt_regs->msr_FP=%d "
                                 "ckpt_regs->msr_VMX=%d "
                                 "ckpt_regs->msr_VSX=%d "
                              "ckpt_regs->msr_MSR_TS=%d "
                              "ckpt_regs->msr_MSR_TM=%d "
                                  "ckvr_state->vr[0]=%p",

                                            cpu(), tid(),

                          task_current()->thread->load_fp,
                         task_current()->thread->load_vec,
                          task_current()->thread->used_vr,
                         task_current()->thread->used_vsr,
                       task_current()->thread->regs->trap,

                                    // Read from register.
                                          register("msr"), // MSR
                        register("msr") & 1 << 13 ? 1 : 0, // MSR_FP
                        register("msr") & 1 << 25 ? 1 : 0, // MSR_VEC
                        register("msr") & 1 << 23 ? 1 : 0, // MSR_VSX
                        register("msr") & (1 << 33 |1 << 34) ? (register("msr") & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                          register("msr") & 1<<32 ? 1 : 0,

                                // Read from thread->regs.
                        task_current()->thread->regs->msr,  // MSR
      task_current()->thread->regs->msr & 1 << 13 ? 1 : 0,  // MSR_FP
      task_current()->thread->regs->msr & 1 << 25 ? 1 : 0,  // MSR_VEC
      task_current()->thread->regs->msr & 1 << 23 ? 1 : 0,  // MSR_VSX
      task_current()->thread->regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
      task_current()->thread->regs->msr & 1 << 32 ? 1 : 0,  // MSR_TM
      task_current()->thread->vr_state->vr, // vr_state->vr[0]

                           // Read from thread->ckpt_regs.
                   task_current()->thread->ckpt_regs->msr,  // MSR
 task_current()->thread->ckpt_regs->msr & 1 << 13 ? 1 : 0,  // MSR_FP
 task_current()->thread->ckpt_regs->msr & 1 << 25 ? 1 : 0,  // MSR_VEC
 task_current()->thread->ckpt_regs->msr & 1 << 23 ? 1 : 0,  // MSR_VSX
 task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34) ? (task_current()->thread->ckpt_regs->msr & (1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
 task_current()->thread->ckpt_regs->msr & 1 << 32 ? 1 : 0, // MSR_TM
                  task_current()->thread->ckvr_state->vr);

 // Print vsx32 (aka vmx0) using hackish version of print_regs().
 print_regs();
  }
}

