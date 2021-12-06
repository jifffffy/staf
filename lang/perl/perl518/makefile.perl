#############################################################################
# Software Testing Automation Framework (STAF)                              #
#                                                                           #
#                                                                           #
# This software is licensed under the Eclipse Public License (EPL) V1.0.    #
#############################################################################

ifeq ($(PERL_BUILD_V518),1)

OS_COMMONFLAGS += -DPERL5_6PLUS

LIB_PLSTAF = $(subst Name,PLSTAF,$(DLL))

SUBSYS_REL_PERL = lang/perl/perl518

perl518_targets += $(REL)/lib/perl518/$(LIB_PLSTAF) \
                  $(REL)/bin/PLSTAFService.pm

Targets += $(perl518_targets)
CleanupTargets += cleanup_perl518

$(perl518_targets): SUBSYS_REL := lang/perl/perl518
SUBSYS_REL := lang/perl/perl518

#=====================================================================
#   C/C++ Info Flags
#=====================================================================

$(perl518_targets): INCLUDEDIRS = $(PERL_V518_INCLUDEDIRS) $(SRC)/$(SUBSYS_REL)/..
$(perl518_targets): OBJS        = $(perl518_objs)
$(perl518_targets): LIBS        = STAF $(PERL_V518_LIBS)
$(perl518_targets): LIBDIRS     = $(PERL_V518_LIBDIRS)
$(perl518_targets): CFLAGS     := $(CC_EXPORT_SHARED_LIB_SYMBOLS)

perl518_objs := \
        STAFPerlService \
        STAFPerlGlue \
        STAFPerlSyncHelper \
        PLSTAF \
        PLSTAFCommandParser

perl518_objs         := $(foreach obj,$(perl518_objs),$(O)/$(SUBSYS_REL)/$(obj)$(OS_OE))
perl518_dependents   := $(perl518_objs:$(OS_OE)=.d)
$(perl518_dependents): SUBSYS_REL := $(SUBSYS_REL_PERL)
$(perl518_dependents): INCLUDEDIRS = $(PERL_V518_INCLUDEDIRS) $(STAFDIR)

ifeq ($(OS_NAME),win32)
    perl518_objs += $(SRC)/lang/perl/PLSTAF.def
endif

# Include dependencies
ifneq ($(InCleanup), "1")
	include $(perl518_dependents)
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

$(REL)/lib/perl518/$(LIB_PLSTAF): $(perl518_objs) $(LIB_STAF_FP) $(MAKEFILE_NAME)
	$(SHARED_LIB_IT)

$(REL)/bin/PLSTAFService.pm:      $(SRC)/lang/perl/PLSTAFService.pm       $(MAKEFILE_NAME)
	$(COPY_FILE)

cleanup_perl518:
	-@$(DEL) $(O)/$(SUBSYS_REL)/* $(OUT_ERR_TO_DEV_NULL)
	-@$(DEL) $(REL)/lib/perl518/$(LIB_PLSTAF) $(OUT_ERR_TO_DEV_NULL)

endif
