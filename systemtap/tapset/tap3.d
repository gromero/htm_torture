/*
probe kernel.function("vsx_unavailable_tm").call {
  if (execname() == "torture") {
    printf("call: tid: %ld, load_fp=%p, load_vec=%p, trap=%p, msr=%p ",
            ns_tid(), task_current()->thread->load_fp, task_current()->thread->load_vec, $regs->trap, register("msr"));
//  if ($regs->trap == 0x700)
//    print_stack(callers(-1));
    print_regs();
  }
}
*/
/*
probe kernel.function("vsx_unavailable_tm").return {
  if (execname() == "torture") {
    printf("return: tid: %ld, load_fp=%p, load_vec=%p, trap=%p, msr=%p ",
            ns_tid(), task_current()->thread->load_fp, task_current()->thread->load_vec, $regs->trap, register("msr"));
//  if ($regs->trap == 0x700)
//    print_stack(callers(-1));
    print_regs();
  }
}
*/
/*
probe kernel.function("save_all").call {
  if(execname() == "torture") {

    printf("call: load_fp=%p, trap=%p\n", task_current()->thread->load_fp, task_current()->thread->regs->trap);
    print_regs();
    %{

     asm ("ori 0,0,0;");

    %}
  } 
}
probe kernel.function("save_all").return {
  if(execname() == "torture") {
    printf("return: load_fp=%p\n", task_current()->thread->load_fp);
    print_regs();
  } 
}
*/
/*
probe kernel.function("arch_dup_task_struct").call {
  if(execname() == "torture") {

    printf("call: load_fp=%p, trap=%p\n", task_current()->thread->load_fp, task_current()->thread->regs->trap);
    print_regs();
  } 
}
*/
// tm_reclaim_task

probe kernel.function("__switch_to").return {
  if (execname() == "torture") {
    printf("call: tid: %ld, load_fp=%p, load_vec=%p, msr=%p ",
           tid(), task_current()->thread->load_fp, task_current()->thread->load_vec, register("msr"));
//  if ($regs->trap == 0x700)
//    print_stack(callers(-1));
    print_regs();
  }
}
