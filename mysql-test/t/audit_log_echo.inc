# Echo the contents of the audit log (in CSV format)
# log_file is the name of the log file

--move_file $log_file $log_file.copy
set global audit_log_flush= ON;
let log_file=$log_file;
perl;
  print "===================================================================\n";
  open my $file, $ENV{'log_file'} . '.copy' or die "Can not open log: $!";
  while ($line = <$file>) {
    if ($line =~ /SET NAMES/) {
      # change_user does automatic reconnect and messing up 'SET NAMES' around
      next;
    }
    if ($line =~ /Threads_connected/ || $line =~ /SELECT \d <= \d/) {
      # part of waitwait_until_count_sessions.inc script
      next;
    }
    if ($line =~ /^"Audit"/) {
      # skip opening log record
      next;
    }
    $line =~ s/"([a-zA-Z_ ]*)","([0-9]+)_[0-9_ :T-]*","[0-9_ :A-Z-]*"/"$1","<ID>","<DATETIME>"/;
    $line =~ s/"(Connect|Quit|Change user)","<ID>","<DATETIME>","[0-9]+"/"$1","<ID>","<DATETIME>","<CONN_ID>"/;
    $line =~ s/"([A-Za-z ]+)","<ID>","<DATETIME>","([a-z_]+)","[0-9]+"/"$1","<ID>","<DATETIME>","$2","<CONN_ID>"/;
    if ($line =~ /SET GLOBAL .*_accounts/) {
      print "*************************************************************\n";
    }
    print "$line";
  };
  close $file;
  print "===================================================================\n";
EOF
--remove_file $log_file.copy
