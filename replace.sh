#!/bin/bash
make M=arch/x86/kvm
rmmod kvm-intel
rmmod kvm
insmod arch/x86/kvm/kvm.ko
insmod arch/x86/kvm/kvm-intel.ko
