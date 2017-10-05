/* Default program name. Can be overrided by -G command=<new_program_name> */
global command="vsx_una_";

// Use with:
// $ sudo /home/gromero/systemtap/bin/stap  -g --suppress-time-limits -v -r /home/gromero/git/linux ./tapset/signal.d -G command=bad |& tee tap.log

/********************
 ** PRETTY PRINTER **
 ********************/

function pp(function_name: string)
{

 if (execname() == command) {

 try {

  load_fp  = task_current()->thread->load_fp
  load_vec = task_current()->thread->load_vec
  load_tm  = task_current()->thread->load_tm

  tm_texasr = task_current()->thread->tm_texasr

  used_vr  = task_current()->thread->used_vr
  used_vsr = task_current()->thread->used_vsr
  } catch {
    load_fp = 0xbeef
    load_vec = 0xbeef
    load_tm = 0xbeef
    used_vr = 0xbeef
    used_vsr = 0xbeef
  }

  try {
    regs_msr      = task_current()->thread->regs->msr
    regs_nip      = task_current()->thread->regs->nip
    ckpt_regs_msr = task_current()->thread->ckpt_regs->msr
    regs_gpr1 = task_current()->thread->regs->gpr[1]
    ckpt_regs_gpr1 = task_current()->thread->ckpt_regs->gpr[1]
  } catch {
    regs_msr       = 0xbeef
    regs_nip       = 0xbeef
    ckpt_regs_msr  = 0xbeef
    regs_gpr1      = 0xbeef
    ckpt_regs_gpr1 = 0xbeef
  }

  msr = register("msr")

  printf("%s:  cpu: %3ld tid: %6ld ", function_name, cpu(), tid())

//r1 = register("r1")

//printf("r1=0x%.16lx ", r1)
  printf("regs->nip: 0x%.16lx ", regs_nip)
	 
  printf("regs->r1: 0x%.16lx ", regs_gpr1)

  printf("ckpt_regs->r1: 0x%.16lx ", ckpt_regs_gpr1)

  printf("load_fp=0x%.2x load_vec=0x%.2x load_tm=0x%.2x used_vr=%d used_vsr=%d tm_texasr=0x%.16lx ", load_fp, load_vec, load_tm, used_vr, used_vsr, tm_texasr)

  printf("msr[ FP=%d VEC=%d VSX=%d TS=%d TM=%d ]   ", msr & 1 << 13 ? 1 : 0, // MSR_FP
                                                      msr & 1 << 25 ? 1 : 0, // MSR_VEC
                                                      msr & 1 << 23 ? 1 : 0, // MSR_VSX
                                                      msr & (1 << 33 | 1 << 34) ? (msr & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                                                      msr & 1 << 32 ? 1 : 0) // MSR_TM

  printf("regs->msr[ FP=%d VEC=%d VSX=%d TS=%d TM=%d ]   ", regs_msr & 1 << 13 ? 1 : 0, // MSR_FP
                                                            regs_msr & 1 << 25 ? 1 : 0, // MSR_VEC
                                                            regs_msr & 1 << 23 ? 1 : 0, // MSR_VSX
                                                            regs_msr & (1 << 33 | 1 << 34) ? (regs_msr & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                                                            regs_msr & 1 << 32 ? 1 : 0) // MSR_TM

  printf("ckpt_regs->msr[ FP=%d VEC=%d VSX=%d TS=%d TM=%d ]   ", ckpt_regs_msr & 1 << 13 ? 1 : 0, // MSR_FP
                                                                 ckpt_regs_msr & 1 << 25 ? 1 : 0, // MSR_VEC
                                                                 ckpt_regs_msr & 1 << 23 ? 1 : 0, // MSR_VSX
                                                                 ckpt_regs_msr & (1 << 33 | 1 << 34) ? (ckpt_regs_msr & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                                                                 ckpt_regs_msr & 1 << 32 ? 1 : 0) // MSR_TM

  try { 
    printf("trap=0x%.3x ", task_current()->thread->regs->trap);
  } catch {
    printf("trap=0x%.3x ", 0xbee);
  }

  printf("\n")
 }
}

/*
function pp_(function_name: string, t: long, original_msr: long)
{

  load_fp = @cast(t, "task_struct", "kernel<linux/sched.h>")->thread->load_fp

  print(load_fp);

  

//  load_fp  = task_current()->thread->load_fp
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
  // print_regs()
  //print_backtrace();
  printf("\n");
}
*/

/*
function pp__(function_name: string, prev_task: long, new_task: long)
{
   if (prev_task == new_task) {
     printf("%s: warning: prev task == new task: 0x%.16lx, task_execname(): %s, see backtrace below:\n", function_name, prev_task, task_execname(prev_task));
     print_backtrace();
//   printf("%s: prev=0x%.16lx, task_execname(): %s\n", function_name, prev_task, task_execname(prev_task));
//   printf("%s: new =0x%.16lx, task_execname(): %s\n", function_name, new_task , task_execname(new_task) );
   } 

   if (task_execname(prev_task) == command || task_execname(new_task) == command) {


//  prev_execname = task_execname(prev_task)
//  new_execname  = task_execname(new_task)

//  if (prev_execname == command)
//  	printf("prev_execname=%s\n", prev_execname)
//  if (new_execname == command)
//  	printf("new_execname=%s\n", new_execname)


  msr           = register("msr")

  printf("%s: ", function_name)

  printf("       msr [ FP=%d VEC=%d VSX=%d TS=%d TM=%d ] \n", msr & 1 << 13 ? 1 : 0, // MSR_FP
                                                       msr & 1 << 25 ? 1 : 0, // MSR_VEC
                                                       msr & 1 << 23 ? 1 : 0, // MSR_VSX
                                                       msr & (1 << 33 | 1 << 34) ? (msr & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                                                       msr & 1 << 32 ? 1 : 0) // MSR_TM

  // PREV TASK  

  p_load_fp =  @cast(prev_task, "task_struct", "kernel<linux/sched.h>")->thread->load_fp
  p_load_vec = @cast(prev_task, "task_struct", "kernel<linux/sched.h>")->thread->load_vec
  p_load_tm  = @cast(prev_task, "task_struct", "kernel<linux/sched.h>")->thread->load_tm

  p_tm_texasr  = @cast(prev_task, "task_struct", "kernel<linux/sched.h>")->thread->tm_texasr

  p_used_vr  = @cast(prev_task, "task_struct", "kernel<linux/sched.h>")->thread->used_vr
  p_used_vsr = @cast(prev_task, "task_struct", "kernel<linux/sched.h>")->thread->used_vsr

  try {
  p_regs_msr      = @cast(prev_task, "task_struct", "kernel<linux/sched.h>")->thread->regs->msr
  p_ckpt_regs_msr = @cast(prev_task, "task_struct", "kernel<linux/sched.h>")->thread->ckpt_regs->msr
  p_regs_gpr1     = @cast(prev_task, "task_struct", "kernel<linux/sched.h>")->thread->regs->gpr[1]
  p_ckpt_regs_gpr1= @cast(prev_task, "task_struct", "kernel<linux/sched.h>")->thread->ckpt_regs->gpr[1]
  p_regs_trap     = @cast(prev_task, "task_struct", "kernel<linux/sched.h>")->thread->regs->trap
  p_ckpt_regs_trap= @cast(prev_task, "task_struct", "kernel<linux/sched.h>")->thread->ckpt_regs->trap
  } catch {
    p_regs_msr = 0xbeef
    p_ckpt_regs_msr = 0xbeef
    p_regs_gpr1 = 0xbeef
    p_ckpt_regs_gpr1 = 0xbeef
    p_regs_trap = 0xbee
    p_ckpt_regs_trap = 0xbee
  } 

  printf("|     prev task: ")

  printf("cpu: %3ld tid: %6ld ", task_cpu(prev_task), task_tid(prev_task))

  printf("r1=0x%.16lx ", register("r1"));

  printf("regs->r1: 0x%.16lx ", p_regs_gpr1)

  printf("ckpt_regs->r1: 0x%.16lx ", p_ckpt_regs_gpr1)

  printf("load_fp=0x%.2x load_vec=0x%.2x load_tm=0x%.2x used_vr=%d used_vsr=%d tm_texasr=0x%.16lx ", p_load_fp, p_load_vec, p_load_tm, p_used_vr, p_used_vsr, p_tm_texasr)

  printf("regs->msr [ FP=%d VEC=%d VSX=%d TS=%d TM=%d ] ", p_regs_msr & 1 << 13 ? 1 : 0, // MSR_FP
                                                           p_regs_msr & 1 << 25 ? 1 : 0, // MSR_VEC
                                                           p_regs_msr & 1 << 23 ? 1 : 0, // MSR_VSX
                                                           p_regs_msr & (1 << 33 | 1 << 34) ? (p_regs_msr & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                                                           p_regs_msr & 1 << 32 ? 1 : 0) // MSR_TM

  printf("ckpt_regs->msr [ FP=%d VEC=%d VSX=%d TS=%d TM=%d ] ", p_ckpt_regs_msr & 1 << 13 ? 1 : 0, // MSR_FP
                                                                p_ckpt_regs_msr & 1 << 25 ? 1 : 0, // MSR_VEC
                                                                p_ckpt_regs_msr & 1 << 23 ? 1 : 0, // MSR_VSX
                                                                p_ckpt_regs_msr & (1 << 33 | 1 << 34) ? (p_ckpt_regs_msr & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                                                                p_ckpt_regs_msr & 1 << 32 ? 1 : 0) // MSR_TM
   
  printf("regs->trap=0x%.3x, ckpt_regs->0x%.3x", p_regs_trap, p_ckpt_regs_trap);
 
  printf("\n");

  // NEW TASK  

  n_load_fp =  @cast(new_task, "task_struct", "kernel<linux/sched.h>")->thread->load_fp
  n_load_vec = @cast(new_task, "task_struct", "kernel<linux/sched.h>")->thread->load_vec
  n_load_tm  = @cast(new_task, "task_struct", "kernel<linux/sched.h>")->thread->load_tm

  n_tm_texasr  = @cast(new_task, "task_struct", "kernel<linux/sched.h>")->thread->tm_texasr

  n_used_vr  = @cast(new_task, "task_struct", "kernel<linux/sched.h>")->thread->used_vr
  n_used_vsr = @cast(new_task, "task_struct", "kernel<linux/sched.h>")->thread->used_vsr

  try {
    n_regs_msr      = @cast(new_task, "task_struct", "kernel<linux/sched.h>")->thread->regs->msr
    n_ckpt_regs_msr = @cast(new_task, "task_struct", "kernel<linux/sched.h>")->thread->ckpt_regs->msr
    n_regs_gpr1     = @cast(new_task, "task_struct", "kernel<linux/sched.h>")->thread->regs->gpr[1]
    n_ckpt_regs_gpr1= @cast(new_task, "task_struct", "kernel<linux/sched.h>")->thread->ckpt_regs->gpr[1]
    n_regs_trap     = @cast(new_task, "task_struct", "kernel<linux/sched.h>")->thread->regs->trap
    n_ckpt_regs_trap= @cast(new_task, "task_struct", "kernel<linux/sched.h>")->thread->ckpt_regs->trap
  } catch {
    n_regs_msr = 0xBEEF
    n_ckpt_regs_msr = 0xBEEF
    n_regs_gpr1 = 0xBEEF
    n_ckpt_regs_gpr1 = 0xBEEF
    n_regs_trap = 0xBEE
    n_ckpt_regs_trap = 0xBEE
  } 
 
  printf("|     new  task: ")

  printf("cpu: %3ld tid: %6ld ", task_cpu(new_task), task_tid(new_task))

  printf("r1=0x%.16lx ", register("r1"));

  printf("regs->r1: 0x%.16lx ", n_regs_gpr1)

  printf("ckpt_regs->r1: 0x%.16lx ", n_ckpt_regs_gpr1)

  printf("load_fp=0x%.2x load_vec=0x%.2x load_tm=0x%.2x used_vr=%d used_vsr=%d tm_texasr=0x%.16lx ", n_load_fp, n_load_vec, n_load_tm, n_used_vr, n_used_vsr, n_tm_texasr)

  printf("regs->msr [ FP=%d VEC=%d VSX=%d TS=%d TM=%d ] ", n_regs_msr & 1 << 13 ? 1 : 0, // MSR_FP
                                                           n_regs_msr & 1 << 25 ? 1 : 0, // MSR_VEC
                                                           n_regs_msr & 1 << 23 ? 1 : 0, // MSR_VSX
                                                           n_regs_msr & (1 << 33 | 1 << 34) ? (n_regs_msr & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                                                           n_regs_msr & 1 << 32 ? 1 : 0) // MSR_TM

  printf("ckpt_regs->msr [ FP=%d VEC=%d VSX=%d TS=%d TM=%d ] ", n_ckpt_regs_msr & 1 << 13 ? 1 : 0, // MSR_FP
                                                                n_ckpt_regs_msr & 1 << 25 ? 1 : 0, // MSR_VEC
                                                                n_ckpt_regs_msr & 1 << 23 ? 1 : 0, // MSR_VSX
                                                                n_ckpt_regs_msr & (1 << 33 | 1 << 34) ? (n_ckpt_regs_msr & ( 1 << 33 | 1 << 34)) >> 33 : 0, // MSR_TS (2 bits)
                                                                n_ckpt_regs_msr & 1 << 32 ? 1 : 0) // MSR_TM

  printf("regs->trap=0x%.3x, ckpt_regs->0x%.3x", n_regs_trap, n_ckpt_regs_trap);

  printf("\n");
 }
}
*/

/*************
 ** PROBES ***
 *************/
/*
probe kernel.function("__switch_to_tm").call              { pp__("__switch_to_tm.call          ", $prev, $new) }
probe kernel.function("__switch_to_tm").return            { pp__("__switch_to_tm.return        ", @entry($prev), @entry($new)) }

probe kernel.function("__switch_to").call                 { pp("__switch_to.call                ") }
probe kernel.function("__switch_to").return               { pp("__switch_to.return              ") }

probe kernel.function("restore_tm_sigcontexts").inline    { pp("restore_tm_sigcontexts.inline   ") }
probe kernel.function("setup_tm_sigcontexts").inline      { pp("setup_tm_sigcontexts.inline     ") }
probe kernel.function("tm_reclaim").call                  { pp("tm_reclaim.call                 ") }
probe kernel.function("tm_reclaim").return                { pp("tm_reclaim.return               ") }
probe kernel.function("tm_reclaim_task").inline           { pp("tm_reclaim_task.inline          ") }
probe kernel.function("tm_reclaim_thread").call           { pp("tm_reclaim_thread.call          ") }
probe kernel.function("tm_reclaim_thread").return         { pp("tm_reclaim_thread.return        ") }
probe kernel.function("tm_reclaim_current").call          { pp("tm_reclaim_current.call         "); }
probe kernel.function("tm_reclaim_current").return        { pp("tm_reclaim_current.return       "); }
*/
//probe kernel.function("tm_recheckpoint_new_task").inline  { pp("tm_recheckpoint_new_task.inline ") }

// probe kernel.function("tm_recheckpoint").call             { pp_("tm_recheckpoint.call            ", thread, orig_msr) }
// probe kernel.function("tm_recheckpoint").return           { pp_("tm_recheckpoint.return          ", thread, orig_msr) }
probe kernel.function("tm_recheckpoint").call                { pp("tm_recheckpoint.call            "); print_backtrace(); }
probe kernel.function("tm_recheckpoint").return              { pp("tm_recheckpoint.return          "); print_backtrace(); }
probe kernel.function("get_tm_stackpointer").call            { pp("get_tm_stackpointer.call        "); print_backtrace(); }
probe kernel.function("get_tm_stackpointer").return          { pp("get_tm_stackpointer.return      "); print_backtrace(); }
// probe kernel.function("tm_reclaim").call                     { pp("tm_reclaim.call                 ") }
// probe kernel.function("tm_reclaim").return                   { pp("tm_reclaim.return               ") }
// probe kernel.function("check_if_tm_restore_required").call   { pp("check_if_tm_restore_required.cl ") }
// probe kernel.function("check_if_tm_restore_required").return { pp("check_if_tm_restore_required.rt ") }


// probe kernel.function("sys_rt_sigreturn").call	          { pp("sys_rt_sigreturn.call           "); print_backtrace(); }
// probe kernel.function("sys_rt_sigreturn").return	  { pp("sys_rt_sigreturn.return         ") }

//probe kernel.function("load_up_fpu").call                 { pp("load_up_fpu.call                ") }
//probe kernel.function("load_up_fpu").return               { pp("load_up_fpu.return              ") }
// probe kernel.function("fp_unavailable_tm").call           { pp("fp_unavailable_tm.call          ") }
// probe kernel.function("fp_unavailable_tm").return         { pp("fp_unavailable_tm.return        ") }

// probe kernel.function("load_up_altivec").call             { pp("load_up_altivec.call            ") }
// probe kernel.function("load_up_altivec").return           { pp("load_up_altivec.return          ") }
// probe kernel.function("altivec_unavailable_tm").call      { pp("altivec_unavailable_tm.call     ") }
// probe kernel.function("altivec_unavailable_tm").return    { pp("altivec_unavailable_tm.return   ") }

//probe kernel.function("load_up_vsx").call                 { pp("load_up_vsx.call                ") }
//probe kernel.function("load_up_vsx").return               { pp("load_up_vsx.return              ") }
// probe kernel.function("vsx_unavailable_tm").call          { pp("vsx_unavailable_tm.call         ") }
// probe kernel.function("vsx_unavailable_tm").return        { pp("vsx_unavailable_tm.return       ") }
// probe kernel.function("restore_math").call                { pp("restore_math.call               ") }
// probe kernel.function("restore_math").return              { pp("restore_math.return             ") }

// probe kernel.function("tm_unavailable").inline            { pp("tm_unavailable.inline           ") }
probe kernel.function("handle_rt_signal64").call             { pp("handle_rt_signal64.call         "); print_backtrace(); }
probe kernel.function("handle_rt_signal64").return           { pp("handle_rt_signal64.return       "); print_backtrace(); }
probe kernel.function("setup_tm_sigcontexts")                { pp("setup_tm_sigcontexts.call       "); print_backtrace(); }
probe kernel.function("setup_sigcontext")                    { pp("setup_sigcontext.call           "); print_backtrace(); }

// setup_tm_sigcontexts
// setup_sigcontext
//probe kernel.function("program_check_exception").call     { pp("program_check_exception.call    ") }

//probe kernel.statement(0xc000000000051258).absolute       { pp("tm_reclaim.just_after_reclaim   ") }
// probe kernel.function("fast_exception_return").call       { pp("fast_exception_return.call      ") }
// probe kernel.function("fast_exception_return").return     { pp("fast_exception_return.return    ") }
//probe kernel.statement(0xc00000000000b7d0).absolute         { pp("fast_exception_return.calll     ") }
