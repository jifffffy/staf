#############################################################################
# Software Testing Automation Framework (STAF)                              #
#                                                                           #
#                                                                           #
# This software is licensed under the Eclipse Public License (EPL) V1.0.    #
#############################################################################

ifeq ($(PERL_BUILD_V520),1)

OS_COMMONFLAGS += -DPERL5_6PLUS

LIB_PLSTAF = $(subst Name,PLSTAF,$(DLL))

SUBSYS_REL_PERL = lang/perl/perl520

perl520_targets += $(REL)/lib/perl520/$(LIB_PLSTAF) \
                  $(REL)/bin/PLSTAFService.pm

Targets += $(perl520_targets)
CleanupTargets += cleanup_perl520

$(perl520_targets): SUBSYS_REL := lang/perl/perl520
SUBSYS_REL := lang/perl/perl520

#=====================================================================
#   C/C++ Info Flags
#=====================================================================

$(perl520_targets): INCLUDEDIRS = $(PERL_V520_INCLUDEDIRS) $(SRC)/$(SUBSYS_REL)/..
$(perl520_targets): OBJS        = $(perl520_objs)
$(perl520_targets): LIBS        = STAF $(PERL_V520_LIBS)
$(perl520_targets): LIBDIRS     = $(PERL_V520_LIBDIRS)
$(perl520_targets): CFLAGS     := $(CC_EXPORT_SHARED_LIB_SYMBOLS)

perl520_objs := \
        STAFPerlService \
        STAFPerlGlue \
        STAFPerlSyncHelper \
        PLSTAF \
        PLSTAFCommandParser

perl520_objs         := $(foreach obj,$(perl520_objs),$(O)/$(SUBSYS_REL)/$(obj)$(OS_OE))
perl520_dependents   := $(perl520_objs:$(OS_OE)=.d)
$(perl520_dependents): SUBSYS_REL := $(SUBSYS_REL_PERL)
$(perl520_dependents): INCLUDEDIRS = $(PERL_V520_INCLUDEDIRS) $(STAFDIR)

ifeq ($(OS_NAME),win32)
    perl520_objs += $(SRC)/lang/perl/PLSTAF.def
endif

# Include dependencies
ifneq ($(InCleanup), "1")
	include $(perl520_dependents)
endif

# Include inference rules
include $(InferenceRules)

# These two rules allow us to build the Perl specific versions from the common
# parent tree

$(O)/$(SUBSYS_REL)/%.d: $(SRC)/$(SUBSYS_REL)/../%.cpp
	$(C_DEPEND_IT)

$(O)/$(SUBSYS_REL)/%$(OS_OE): $(SRC)/$(SUBSYS_REL)/../%.cpp
	$(COMPILE_IT)

# PERL targets

$(REL)/lib/perl520/$(LIB_PLSTAF): $(perl520_objs) $(LIB_STAF_FP) $(MAKEFILE_NAME)
	$(SHARED_LIB_IT)

$(REL)/bin/PLSTAFService.pm:      $(SRC)/lang/perl/PLSTAFService.pm       $(MAKEFILE_NAME)
	$(COPY_FILE)

cleanup_perl520:
	-@$(DEL) $(O)/$(SUBSYS_REL)/* $(OUT_ERR_TO_DEV_NULL)
	-@$(DEL) $(REL)/lib/perl520/$(LIB_PLSTAF) $(OUT_ERR_TO_DEV_NULL)

endif
