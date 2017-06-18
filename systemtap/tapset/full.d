/* Default program name. Can be overrided by -G command=<new_program_name> */
global command="vsx_una_";


/********************
 ** PRETTY PRINTER **
 ********************/

function pp(function_name: string)
{
 if (execname() == command) {

  load_fp  = task_current()->thread->load_fp
  load_vec = task_current()->thread->load_vec
  load_tm  = task_current()->thread->load_tm

  used_vr  = task_current()->thread->used_vr
  used_vsr = task_current()->thread->used_vsr

  msr           = register("msr")
  regs_msr      = task_current()->thread->regs->msr
  ckpt_regs_msr = task_current()->thread->ckpt_regs->msr

  vr_state_vr0_LOW  = kernel_long((task_current()->thread->vr_state->vr)+0)
  vr_state_vr0_HIGH = kernel_long((task_current()->thread->vr_state->vr)+8)

  ckvr_state_vr0_LOW  = kernel_long((task_current()->thread->ckvr_state->vr)+0)
  ckvr_state_vr0_HIGH = kernel_long((task_current()->thread->ckvr_state->vr)+8)

  printf("%s:  cpu: %3ld tid: %6ld ", function_name, cpu(), tid())

  printf("load_fp=0x%.2x load_vec=0x%.2x load_tm=0x%.2x used_vr=%d used_vsr=%d   ", load_fp, load_vec, load_tm, used_vr, used_vsr)

//printf("msr=%.16p ", msr)
  printf("msr[ FP=%d VEC=%d VSX=%d TS=%d TM=%d ]   ", msr & 1 << 13 ? 1 : 0, // MSR_FP
                                                      msr & 1 << 25 ? 1 : 0, // MSR_VEC
                                                      msr & 1 << 23 ? 1 : 0, // MSR_VSX
                                                      msr & (1 << 33 | 1 << 34) ? (msr & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                                                      msr & 1 << 32 ? 1 : 0) // MSR_TM

//printf("regs->msr=%.16p ", regs_msr)
  printf("regs->msr[ FP=%d VEC=%d VSX=%d TS=%d TM=%d ]   ", regs_msr & 1 << 13 ? 1 : 0, // MSR_FP
                                                            regs_msr & 1 << 25 ? 1 : 0, // MSR_VEC
                                                            regs_msr & 1 << 23 ? 1 : 0, // MSR_VSX
                                                            regs_msr & (1 << 33 | 1 << 34) ? (regs_msr & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                                                            regs_msr & 1 << 32 ? 1 : 0) // MSR_TM

//printf("ckpt_regs->msr=%.16p ", ckpt_regs_msr)
  printf("ckpt_regs->msr[ FP=%d VEC=%d VSX=%d TS=%d TM=%d ]   ", ckpt_regs_msr & 1 << 13 ? 1 : 0, // MSR_FP
                                                                 ckpt_regs_msr & 1 << 25 ? 1 : 0, // MSR_VEC
                                                                 ckpt_regs_msr & 1 << 23 ? 1 : 0, // MSR_VSX
                                                                 ckpt_regs_msr & (1 << 33 | 1 << 34) ? (ckpt_regs_msr & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                                                                 ckpt_regs_msr & 1 << 32 ? 1 : 0) // MSR_TM

  printf("vr_state.vr[0]=0x%.16x%.16x ",    vr_state_vr0_HIGH  , vr_state_vr0_LOW)

  printf("ckvr_state.vr[0]=0x%.16x%.16x ", ckvr_state_vr0_HIGH, ckvr_state_vr0_LOW)

  // Print vsx32 (vmx0). It must be the last printing since it contains the \n.
  print_regs()
 }
}

function pp_(function_name: string)
{
  load_fp  = task_current()->thread->load_fp
  load_vec = task_current()->thread->load_vec
  load_tm  = task_current()->thread->load_tm

  used_vr  = task_current()->thread->used_vr
  used_vsr = task_current()->thread->used_vsr

  msr           = register("msr")
  regs_msr      = task_current()->thread->regs->msr
  ckpt_regs_msr = task_current()->thread->ckpt_regs->msr

  vr_state_vr0_LOW  = kernel_long((task_current()->thread->vr_state->vr)+0)
  vr_state_vr0_HIGH = kernel_long((task_current()->thread->vr_state->vr)+8)

  ckvr_state_vr0_LOW  = kernel_long((task_current()->thread->ckvr_state->vr)+0)
  ckvr_state_vr0_HIGH = kernel_long((task_current()->thread->ckvr_state->vr)+8)

  printf("%s:  cpu: %3ld tid: %6ld ", function_name, cpu(), tid())

  printf("load_fp=0x%.2x load_vec=0x%.2x load_tm=0x%.2x used_vr=%d used_vsr=%d   ", load_fp, load_vec, load_tm, used_vr, used_vsr)

//printf("msr=%.16p ", msr)
  printf("msr[ FP=%d VEC=%d VSX=%d TS=%d TM=%d ]   ", msr & 1 << 13 ? 1 : 0, // MSR_FP
                                                      msr & 1 << 25 ? 1 : 0, // MSR_VEC
                                                      msr & 1 << 23 ? 1 : 0, // MSR_VSX
                                                      msr & (1 << 33 | 1 << 34) ? (msr & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                                                      msr & 1 << 32 ? 1 : 0) // MSR_TM

//printf("regs->msr=%.16p ", regs_msr)
  printf("regs->msr[ FP=%d VEC=%d VSX=%d TS=%d TM=%d ]   ", regs_msr & 1 << 13 ? 1 : 0, // MSR_FP
                                                            regs_msr & 1 << 25 ? 1 : 0, // MSR_VEC
                                                            regs_msr & 1 << 23 ? 1 : 0, // MSR_VSX
                                                            regs_msr & (1 << 33 | 1 << 34) ? (regs_msr & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                                                            regs_msr & 1 << 32 ? 1 : 0) // MSR_TM

//printf("ckpt_regs->msr=%.16p ", ckpt_regs_msr)
  printf("ckpt_regs->msr[ FP=%d VEC=%d VSX=%d TS=%d TM=%d ]   ", ckpt_regs_msr & 1 << 13 ? 1 : 0, // MSR_FP
                                                                 ckpt_regs_msr & 1 << 25 ? 1 : 0, // MSR_VEC
                                                                 ckpt_regs_msr & 1 << 23 ? 1 : 0, // MSR_VSX
                                                                 ckpt_regs_msr & (1 << 33 | 1 << 34) ? (ckpt_regs_msr & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                                                                 ckpt_regs_msr & 1 << 32 ? 1 : 0) // MSR_TM

  printf("vr_state.vr[0]=0x%.16x%.16x ",    vr_state_vr0_HIGH  , vr_state_vr0_LOW)

  printf("ckvr_state.vr[0]=0x%.16x%.16x ", ckvr_state_vr0_HIGH, ckvr_state_vr0_LOW)

  // Print vsx32 (vmx0). It must be the last printing since it contains the \n.
  print_regs()
}

/*************
 ** PROBES ***
 *************/

probe kernel.function("__switch_to").call                 { pp("__switch_to.call                ") }
probe kernel.function("__switch_to").return               { pp("__switch_to.return              ") }
probe kernel.function("tm_reclaim").call                  { pp("tm_reclaim.call                 ") }
probe kernel.function("tm_reclaim").return                { pp("tm_reclaim.return               ") }
//probe kernel.function("tm_reclaim_task").inline           { pp("tm_reclaim_task.inline          ") }
probe kernel.function("tm_reclaim_thread").call           { pp("tm_reclaim_thread.call          ") }
probe kernel.function("tm_reclaim_thread").return         { pp("tm_reclaim_thread.return        ") }
// probe kernel.function("tm_recheckpoint_new_task").inline  { pp("tm_recheckpoint_new_task.inline ") }
probe kernel.function("tm_recheckpoint").call             { pp_("tm_recheckpoint.call            ") }
probe kernel.function("tm_recheckpoint").return           { pp_("tm_recheckpoint.return          ") }

probe kernel.function("load_up_fpu").call                 { pp("load_up_fpu.call                ") }
probe kernel.function("load_up_fpu").return               { pp("load_up_fpu.return              ") }
probe kernel.function("fp_unavailable_tm").call           { pp("fp_unavailable_tm.call          ") }
probe kernel.function("fp_unavailable_tm").return         { pp("fp_unavailable_tm.return        ") }

probe kernel.function("load_up_altivec").call             { pp("load_up_altivec.call            ") }
probe kernel.function("load_up_altivec").return           { pp("load_up_altivec.return          ") }
probe kernel.function("altivec_unavailable_tm").call      { pp("altivec_unavailable_tm.call     ") }
probe kernel.function("altivec_unavailable_tm").return    { pp("altivec_unavailable_tm.return   ") }

probe kernel.function("load_up_vsx").call                 { pp("load_up_vsx.call                ") }
probe kernel.function("load_up_vsx").return               { pp("load_up_vsx.return              ") }
probe kernel.function("vsx_unavailable_tm").call          { pp("vsx_unavailable_tm.call         ") }
probe kernel.function("vsx_unavailable_tm").return        { pp("vsx_unavailable_tm.return       ") }
probe kernel.function("restore_math").call                { pp("restore_math.call               ") }
probe kernel.function("restore_math").return              { pp("restore_math.return             ") }
