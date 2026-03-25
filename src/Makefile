# Compiladores
CXX = g++
CF = gfortran

# Otimização e Debug
debug: CXX_OPTIMIZATION_FLAGS = -O0
release: CXX_OPTIMIZATION_FLAGS = -O3

debug: F90_OPTIMIZATION_FLAGS = -O0
release: F90_OPTIMIZATION_FLAGS = -O0

debug: DEBUG_FLAGS = -g3
release: DEBUG_FLAGS =

WARNING_FLAGS = -Wall

# Diretório de objetos
OBJDIR = obj

# Flags para compilação
CXXFLAGS = -std=c++11 $(CXX_OPTIMIZATION_FLAGS) $(DEBUG_FLAGS) $(WARNING_FLAGS) -c -static-libgcc -static-libstdc++ -fopenmp
F90FLAGS = -J$(OBJDIR) -c $(F90_OPTIMIZATION_FLAGS) -g -fmax-errors=3 -ffree-line-length-none -fopenmp -lquadmath

# Flags para linkagem
# No Windows, -static garante que libgomp e outras DLLs do MinGW sejam embutidas
ifeq ($(OS),Windows_NT)
    LDFLAGS = -static -fopenmp -lgfortran -lquadmath
else
    LDFLAGS = -static-libgcc -static-libstdc++ -fopenmp -lgfortran -lquadmath
endif

# Arquivos fontes
CXX_SRCS := $(wildcard *.cpp)

F90_SRCS := Constants.f90 Newton.f90 PvtSimWAXFileImport.f90 PvtSimCTMFileImport.f90 \
            VLECalculations.f90 PhaseProperties.f90 Obsoletes.f90 FlashExtras.f90 \
            BlackOilFromCompositional.f90 MarlimComposicional.f90 ViscosityCalculationModule.f90

# Objetos
CXX_OBJS := $(patsubst %.cpp,$(OBJDIR)/%.o,$(CXX_SRCS))
F90_OBJS := $(patsubst %.f90,$(OBJDIR)/%.o,$(F90_SRCS))
OBJS := $(CXX_OBJS) $(F90_OBJS)

# Executável final
OUTPUT = Marlim3

# Alvo principal
all: release

debug: gera_versao create_obj_dir $(OBJS)
	$(CXX) $(OBJS) -o $(OUTPUT) $(LDFLAGS)

release: gera_versao create_obj_dir $(OBJS)
	@echo ">>> Compilando em modo RELEASE (C++ otimizado, Fortran debug)"
	$(CXX) $(OBJS) -o $(OUTPUT) $(LDFLAGS)
	@command -v upx >/dev/null 2>&1 && \
		echo ">>> Compactando com UPX..." && \
		upx $(OUTPUT) || \
		echo ">>> UPX não encontrado, binário não será compactado."

# Compilação de C++
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Compilação de Fortran
$(OBJDIR)/%.o: %.f90
	@mkdir -p $(dir $@)
	$(CF) $(F90FLAGS) -o $@ $<

# Dependências explícitas entre módulos Fortran (necessário para compilação paralela e ordem correta)
$(OBJDIR)/Newton.o: $(OBJDIR)/Constants.o
$(OBJDIR)/VLECalculations.o: $(OBJDIR)/Constants.o
$(OBJDIR)/PvtSimWAXFileImport.o: $(OBJDIR)/Constants.o
$(OBJDIR)/PvtSimCTMFileImport.o: $(OBJDIR)/Constants.o
$(OBJDIR)/ViscosityCalculationModule.o: $(OBJDIR)/Constants.o
$(OBJDIR)/PhaseProperties.o: $(OBJDIR)/Constants.o $(OBJDIR)/VLECalculations.o
$(OBJDIR)/FlashExtras.o: $(OBJDIR)/VLECalculations.o $(OBJDIR)/Newton.o
$(OBJDIR)/Obsoletes.o: $(OBJDIR)/VLECalculations.o $(OBJDIR)/PhaseProperties.o
$(OBJDIR)/BlackOilFromCompositional.o: $(OBJDIR)/VLECalculations.o $(OBJDIR)/PhaseProperties.o $(OBJDIR)/FlashExtras.o
$(OBJDIR)/MarlimComposicional.o: $(OBJDIR)/VLECalculations.o $(OBJDIR)/PvtSimCTMFileImport.o $(OBJDIR)/PhaseProperties.o $(OBJDIR)/BlackOilFromCompositional.o $(OBJDIR)/FlashExtras.o $(OBJDIR)/PvtSimWAXFileImport.o

# Diretório de objetos
create_obj_dir:
	mkdir -p $(OBJDIR)

# Versão
gera_versao:
	bash ./gera_versao.sh

# Limpeza
clean:
	rm -rf $(OBJDIR) $(OUTPUT)
	rm -f *.mod
