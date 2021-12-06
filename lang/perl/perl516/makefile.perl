#############################################################################
# Software Testing Automation Framework (STAF)                              #
#                                                                           #
#                                                                           #
# This software is licensed under the Eclipse Public License (EPL) V1.0.    #
#############################################################################

ifeq ($(PERL_BUILD_V516),1)

OS_COMMONFLAGS += -DPERL5_6PLUS

LIB_PLSTAF = $(subst Name,PLSTAF,$(DLL))

SUBSYS_REL_PERL = lang/perl/perl516

perl516_targets += $(REL)/lib/perl516/$(LIB_PLSTAF) \
                  $(REL)/bin/PLSTAFService.pm

Targets += $(perl516_targets)
CleanupTargets += cleanup_perl516

$(perl516_targets): SUBSYS_REL := lang/perl/perl516
SUBSYS_REL := lang/perl/perl516

#=====================================================================
#   C/C++ Info Flags
#=====================================================================

$(perl516_targets): INCLUDEDIRS = $(PERL_V516_INCLUDEDIRS) $(SRC)/$(SUBSYS_REL)/..
$(perl516_targets): OBJS        = $(perl516_objs)
$(perl516_targets): LIBS        = STAF $(PERL_V516_LIBS)
$(perl516_targets): LIBDIRS     = $(PERL_V516_LIBDIRS)
$(perl516_targets): CFLAGS     := $(CC_EXPORT_SHARED_LIB_SYMBOLS)

perl516_objs := \
        STAFPerlService \
        STAFPerlGlue \
        STAFPerlSyncHelper \
        PLSTAF \
        PLSTAFCommandParser

perl516_objs         := $(foreach obj,$(perl516_objs),$(O)/$(SUBSYS_REL)/$(obj)$(OS_OE))
perl516_dependents   := $(perl516_objs:$(OS_OE)=.d)
$(perl516_dependents): SUBSYS_REL := $(SUBSYS_REL_PERL)
$(perl516_dependents): INCLUDEDIRS = $(PERL_V516_INCLUDEDIRS) $(STAFDIR)

ifeq ($(OS_NAME),win32)
    perl516_objs += $(SRC)/lang/perl/PLSTAF.def
endif

# Include dependencies
ifneq ($(InCleanup), "1")
	include $(perl516_dependents)
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

$(REL)/lib/perl516/$(LIB_PLSTAF): $(perl516_objs) $(LIB_STAF_FP) $(MAKEFILE_NAME)
	$(SHARED_LIB_IT)

$(REL)/bin/PLSTAFService.pm:      $(SRC)/lang/perl/PLSTAFService.pm       $(MAKEFILE_NAME)
	$(COPY_FILE)

cleanup_perl516:
	-@$(DEL) $(O)/$(SUBSYS_REL)/* $(OUT_ERR_TO_DEV_NULL)
	-@$(DEL) $(REL)/lib/perl516/$(LIB_PLSTAF) $(OUT_ERR_TO_DEV_NULL)

endif
