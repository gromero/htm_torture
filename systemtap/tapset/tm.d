
//probe kernel.function("vsx_unavailable_tm").call {
//       printf ("%s -> %s\n", thread_indent(1), probefunc())
//       printf ("%s args [%s]\n", probefunc(), $$parms)
//       printf ("nip: %p\n",   register("nip"));
//}

/*
probe kernel.function("restore_fp").inline {
//       printf("Thread: %s, load_fp on exit: %d\n", thread_indent(1), $$thread->load_fp);
//         printf("Thread: %s, load_fp: %d\n", thread_indent(1), $current->thread.load_fp);
}
*/

// probe kernel.statement("*@kernel/process.c:233") { printf("load_fp: %d\n", @var("current")->load_fp); }

//OK
probe kernel.function("restore_fp").inline {
//    printf("%s, load_fp=%p\n", thread_indent(1), task_current()->thread->load_fp);
//    printf("load_fp=%p\n", task_current()->thread->load_fp);
//      if (execname() == "torture")
//        printf("torture\n");
//    printf("%s\n", execname());
//    print_regs();

      if (execname() == "torture") {
        printf("tid: %ld, load_fp=%p\n", ns_tid(), task_current()->thread->load_fp);
//      printf("tid: %p\n", ns_tid());
      }
}


probe kernel.function("restore_altivec").inline {
  if (execname() == "torture") {
   printf("tid: %ld, load_vec=%p\n", ns_tid(), task_current()->thread->load_vec);
  }
} 
