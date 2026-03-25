/*
 * solverP3Doisson.cpp
 *
 *  Created on: 31 de ago. de 2023
 *      Author: Eduardo
 */
#include "solver3DPoisson.h"

/*void solverP3D::geraNivelFillinILU(
    std::vector<int>& level,
    std::vector<int>& linhaLev,
    std::vector<int>& colunaLev
) {
    const int INF = 1e9;
    LevelMap levelMap;
    int n = malha.nele;

    // Inicializa termos da matriz original com nível 0
    for (int i = 0; i < n; ++i) {
        for (int k = matglob.fnz[i]; k < matglob.fnz[i + 1]; ++k) {
            int j = matglob.clm[k];
            levelMap[key(i, j)] = 0;
        }
    }

    // Iterativamente propaga os níveis até não haver mais mudanças
    bool houveMudanca = true;
    while (houveMudanca) {
        houveMudanca = false;

        // Atualiza a estrutura L[i] a partir de levelMap
        std::vector<std::vector<int>> L(n);
        for (const auto& [chave, nivel] : levelMap) {
            int i = chave / n;
            int j = chave % n;
            L[i].push_back(j);
        }
        for (int i = 0; i < n; ++i) {
            std::sort(L[i].begin(), L[i].end());
            L[i].erase(std::unique(L[i].begin(), L[i].end()), L[i].end()); // remove duplicatas
        }

        // Aplica a regra de propagação
        for (int i = 0; i < n; ++i) {
            for (int k : L[i]) {
                if (k >= i) continue;
                for (int j : L[k]) {
                    if (j <= k) continue;

                    auto chave_ij = key(i, j);
                    auto chave_ik = key(i, k);
                    auto chave_kj = key(k, j);

                    int levij = levelMap.count(chave_ij) ? levelMap[chave_ij] : INF;
                    int levik = levelMap.count(chave_ik) ? levelMap[chave_ik] : INF;
                    int levkj = levelMap.count(chave_kj) ? levelMap[chave_kj] : INF;

                    int novoLev = levik + levkj + 1;
                    if (novoLev < levij && novoLev <= rank) {
                        levelMap[chave_ij] = novoLev;
                        houveMudanca = true;
                    }
                }
            }
        }
    }

    // Converte o resultado final para vetores
    for (const auto& entry : levelMap) {
        int i = entry.first / n;
        int j = entry.first % n;
        linhaLev.push_back(i);
        colunaLev.push_back(j);
        level.push_back(entry.second);
    }
}

void solverP3D::montaILUfillinCSR(
	    const std::vector<int>& level,  const std::vector<int>& linhaLev,const std::vector<int>& colunaLev
	) {
	    noZeroIlu = level.size();
	    veclmILU = new int[noZeroIlu];
	    veclevILU = new int[noZeroIlu];
	    vecRelILU = new int[noZeroIlu];
	    vecfnzILU = new int[malha.nele + 1];

	    int n=malha.nele;

	    // Passo 1: Agrupar termos por linha usando um unordered_map
	       std::unordered_map<int, std::vector<std::pair<int, int>>> termosPorLinha;

	       for (int idx = 0; idx < noZeroIlu; ++idx) {
	           int i = linhaLev[idx];
	           int j = colunaLev[idx];
	           int lev = level[idx];
	           termosPorLinha[i].emplace_back(j, lev);  // Agrupa por linha (CSR)
	       }


	       // Passo 2: Ordenar termos por coluna e preencher os vetores de saída
	       int offset = 0;
	       vecfnzILU[0] = 0;

	       for (int i = 0; i < n; ++i) {
	           if (termosPorLinha.count(i)) {
	               auto& termos = termosPorLinha[i];
	               std::sort(termos.begin(), termos.end());  // Ordena por coluna

	               for (const auto& [j, lev] : termos) {
	                   veclmILU[offset] = j;
	                   veclevILU[offset] = lev;
	                   offset++;
	               }
	           }
	           vecfnzILU[i + 1] = offset;
	       }

	       for (int i = 0; i < n; ++i) {
	    	   int iniLinSp=matglob.fnz[i];
	    	   int fimLinSp=matglob.fnz[i+1];

	    	   int iniLinIlu=vecfnzILU[i];
	    	   int fimLinIlu=vecfnzILU[i+1];

	    	   for(int iILU=iniLinIlu; iILU<fimLinIlu; iILU++){
	    		   vecRelILU[iILU]=-1;
	    		   for(int iSpar=iniLinSp; iSpar<fimLinSp; iSpar++){
	    		   	   if(veclmILU[iILU]==matglob.clm[iSpar]){
	    		   		   vecRelILU[iILU]=iSpar;
	    		   		   break;
	    		   	   }
	    		   }
	    	   }

	       }
	}


std::vector<std::vector<int>> solverP3D::montaForwardDependencias(int N, int* VvecfnzILU, int* VveclmILU) {
    std::vector<std::vector<int>> dependencies(N);
    for (int i = 0; i < N; ++i) {
        for (int j = VvecfnzILU[i]; j < VvecfnzILU[i + 1]; ++j) {
            int col = VveclmILU[j];
            if (col < i) {
                dependencies[i].push_back(col);
            }
        }
        std::sort(dependencies[i].begin(), dependencies[i].end());
    }
    return dependencies;
}

// Calcula cor (nível) da linha i com base nas dependências já resolvidas
int solverP3D::cor_dependenciaForward(int i, const std::vector<std::vector<int>>& dependencias, const std::vector<int>& color) {
    int nivel = 0;
    for (int j : dependencias[i]) {
        nivel = std::max(nivel, color[j] + 1);
    }
    return nivel;
}

std::vector<int> solverP3D::coloreForwardILU() {
	std::vector<std::vector<int>> dependencias;
	if(rank==0){
		dependencias=montaForwardDependencias(dados.nele, matglob.fnz, matglob.clm);
	}
	else{
		dependencias=montaForwardDependencias(dados.nele, vecfnzILU, veclmILU);
	}
	int N = dados.nele;
    std::vector<int> color(N, -1);
    for (int i = 0; i < N; ++i) {
        color[i] = cor_dependenciaForward(i, dependencias, color);
    }
    std::vector<int> colorMenor=color;
    std::sort(colorMenor.begin(), colorMenor.end());
    auto last = std::unique(colorMenor.begin(), colorMenor.end());
    colorMenor.erase(last, colorMenor.end());
    int tamanho=colorMenor.size();

    std::unordered_map<int, int> freq;

     for (int val : color) {
         freq[val]++;
     }

     //for (const auto& [valor, contagem] : freq) {
      //   std::cout << "O valor " << valor << " aparece " << contagem << " vezes.\n";
     //}

    int corLocal;
    for (int i = 0; i < tamanho; ++i) {
    	corLocal=colorMenor[i];
    }
    return color;
}



// Gera dependências superiores (col > i)
std::vector<std::vector<int>> solverP3D::montaBackwardDependencias(int N, int* VvecfnzILU, int* VveclmILU) {
    std::vector<std::vector<int>> dependencias(N);
    for (int i = 0; i < N; ++i) {
        for (int j = VvecfnzILU[i]; j < VvecfnzILU[i + 1]; ++j) {
            int col = VveclmILU[j];
            if (col > i) {
                dependencias[i].push_back(col);
            }
        }
        std::sort(dependencias[i].begin(), dependencias[i].end());
    }
    return dependencias;
}

// Calcula cor (nível) da linha i com base nas dependências já resolvidas
int solverP3D::cor_dependenciaBackward(int i, const std::vector<std::vector<int>>& dependencias, const std::vector<int>& color) {
    int nivel = 0;
    for (int j : dependencias[i]) {
        nivel = std::max(nivel, color[j] + 1);
    }
    return nivel;
}

std::vector<int> solverP3D::coloreBackwardILU() {

	std::vector<std::vector<int>> dependencias;
	if(rank==0){
		dependencias=montaBackwardDependencias(dados.nele, matglob.fnz, matglob.clm);
	}
	else{
		dependencias=montaBackwardDependencias(dados.nele, vecfnzILU, veclmILU);
	}

	int N = dados.nele;
    std::vector<int> color(N, -1);
    for (int i = N - 1; i >= 0; --i) {
        color[i] = cor_dependenciaBackward(i, dependencias, color);
    }

    std::vector<int> colorMenor=color;
    std::sort(colorMenor.begin(), colorMenor.end());
    auto last = std::unique(colorMenor.begin(), colorMenor.end());
    colorMenor.erase(last, colorMenor.end());
    int tamanho=colorMenor.size();

    std::unordered_map<int, int> freq;

    for (int val : color) {
        freq[val]++;
    }

    //for (const auto& [valor, contagem] : freq) {
       // std::cout << "O valor " << valor << " aparece " << contagem << " vezes.\n";
    //}


    int corLocal;
    for (int i = 0; i < tamanho; ++i) {
    	corLocal=colorMenor[i];
    }

    return color;
}*/



solverP3D::solverP3D(string nomeArquivoEntrada,varGlob1D* DGP3DV, int nacopV, vector<DadosGeo>& dutoAux,
		double hi, double he, double ti):
dados(nomeArquivoEntrada),termolivre(dados.nele),matglob(dados.nele,dados.noZero,dados.vecsra,dados.veclm,dados.vecfnz){
	nacop=nacopV;
	itera=0;
	penalizaDt=0;
	//if(nacop>0){
		//dutosMRT = new DadosGeo [nacop];
		//for(int iacop=0; iacop<nacop; iacop++)dutosMRT[iacop]=dutoAux[iacop];
	//}
	DGP3D=DGP3DV;
    (*DGP3D).tempo=0.;

    rank=dados.rank;
    precn=4;
    if(rank<0)precn=1;
    matglob.colorido=dados.colorido;
    matglob.rank=rank;
    //noZeroIlu=0;
    //veclevILU=0;
    //vecRelILU=0;
    //veclmILU=0;
    //vecfnzILU=0;

    malha=malha3d(DGP3D,dados.elementoO, dados.xcoor, dados.noEle, dados.tipo,
    		dados.atributo, 4, dados.nele, dados.nno);
    for(int i=0;i<malha.nele;i++){
    	malha.mlh3d[i].CC=&dados.CC;
    	for(int j=0; j<dados.prop.parserie;j++){
    		double xc=malha.mlh3d[i].cel3D.centroideElem[0];
    		double yc=malha.mlh3d[i].cel3D.centroideElem[1];
    		if(malha.mlh3d[i].eleUNV.regiao==dados.prop.regiao[j]){
    			malha.mlh3d[i].cel3D.cond=dados.prop.cond[j];
    			malha.mlh3d[i].cel3D.cp=dados.prop.cp[j];
    			malha.mlh3d[i].cel3D.rho=dados.prop.rho[j];
    			break;
    		}
    	}
    	//for(int j=0; j<dados.CI.parserie;j++){
    		double xc=malha.mlh3d[i].cel3D.centroideElem[0];
    		double yc=malha.mlh3d[i].cel3D.centroideElem[1];
    		//if(malha.mlh3d[i].eleUNV.regiao==dados.CI.regiao[j]){
    			malha.mlh3d[i].cel3D.tempC=ti;//dados.CI.temp[j];
    			malha.mlh3d[i].cel3D.tempC0=ti;//dados.CI.temp[j];
    			malha.mlh3d[i].cel3D.tempCI=ti;//dados.CI.temp[j];
    			//break;
    		//}
    	//}
    	for(int j=0; j<malha.mlh3d[i].cel3D.nvert;j++){
    		if(malha.mlh3d[i].kvizinho[j]<0){
    			malha.mlh3d[i].atualizaCC(j);
    		}
    	}
    }

    int indsra=0;
    for(int i=0;i<malha.nele;i++){
    	int dimNoZero=malha.mlh3d[i].cel3D.indVizCres.size();
    	//vecfnz[i]=malha.mlh3d[i].cel3D.indVizCres[0];
    	matglob.fnz[i]=indsra;
    	for(int j=0;j<dimNoZero;j++){
    		int ind=malha.mlh3d[i].cel3D.indVizCres[j];
    		matglob.sra[indsra+j]=malha.mlh3d[i].local.mx[0][j];
    		matglob.clm[indsra+j]=ind;
    	}
    	indsra+=dimNoZero;
    	termolivre[i]=malha.mlh3d[i].TL[0];
    }
    matglob.fnz[malha.nele]=dados.noZero;

    if(rank>0){
    	/*vector<int> level;
    	vector<int> colunaLev;
    	vector<int> linhaLev;

    	geraNivelFillinILU(level, linhaLev, colunaLev);
    	montaILUfillinCSR(level, linhaLev, colunaLev);

    	matglob.noZeroIlu=noZeroIlu;
    	matglob.rank=rank;
    	matglob.vecRelILU=vecRelILU;
    	matglob.vecfnzILU=vecfnzILU;
    	matglob.veclevILU=veclevILU;
    	matglob.veclmILU=veclmILU;*/
    	matglob.geraILUrank();
    	matglob.sraILU=new double[matglob.noZeroIlu];
    }
    if(matglob.colorido && rank>=0)matglob.geraILUcor();

	//matglob.colorido=dados.colorido;

    /*if(dados.colorido){
    	// Colorização para L (forward)
    	matglob.color_forward = coloreForwardILU();

    	// Colorização para U (backward)
    	matglob.color_backward = coloreBackwardILU();
    }*/

    if(dados.acop==1){
    	for(int iacop=0;iacop<dados.CC.nAcop;iacop++){
    		dados.hE[iacop]=he;
    		dados.hI[iacop]=hi;
    		dados.tInt[iacop]=ti;
    		/*dados.hE[iacop]=dados.transfer[iacop].hExt(0, dados.diamRef[iacop]);
    		dados.hI[iacop]=dados.transfer[iacop].hInt(0, dados.dutosMRT[iacop].a);
    		dados.condGlob[iacop]=dados.transfer[iacop].condParede();
    		dados.condLoc[iacop]=dados.transfer[iacop].condParedeLocal();
    		double multCond=1;
    		if(dados.soConvInt[iacop]==1)multCond=0.;
    	//tParede=
    		dados.qDesacop[iacop]=((2*M_PI-dados.angAcop)*(dados.tInt[iacop]-dados.tAmb))/(multCond/dados.condGlob[iacop]+
    				1./(0.5*dados.dutosMRT[iacop].a*dados.hI[iacop])+
    			1./(0.5*dados.diamRef[iacop]*dados.hE[iacop]));*/
    		dados.qAcop[iacop]=0.;
    	//por enquanto será retirrado este trecho. ATENCAO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    	/*for(int i=0; i<dados.CC.nRic; i++){
    		for(int j=0; j<dados.CC.ccRic[i].nserie; j++){
    			dados.CC.ccRic[i].valAmb[j]=dados.tAmb;
    			dados.CC.ccRic[i].hAmb[j]=dados.hE;
    		}
    	}*/
    		for(int i=0;i<malha.nele;i++){
    			for(int j=0; j<malha.mlh3d[i].cel3D.nvert;j++){
    				if(malha.mlh3d[i].kvizinho[j]<0){
    					int kcc=0;
    					//while(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
    					while(kcc<dados.CC.nAcop && malha.mlh3d[i].cel3D.ccFace[j]!=dados.CC.rotuloAcop[iacop]) kcc++;
    					if(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]==dados.CC.rotuloAcop[iacop]){
    						malha.mlh3d[i].ccHR[j]=dados.hI[iacop];
    						dados.qAcop[iacop]+=malha.mlh3d[i].cel3D.sFaceMod[j]*
								(dados.tInt[iacop]-malha.mlh3d[i].cel3D.tempC)/(1./(dados.hI[iacop]));
    					}
    				}
    			}
    		}
    		dados.qTotal[iacop]=dados.qAcop[iacop];
    		dados.tParede[iacop]=dados.tInt[iacop];
    		for(int i=0;i<malha.nele;i++){
    			for(int j=0; j<malha.mlh3d[i].cel3D.nvert;j++){
    				if(malha.mlh3d[i].kvizinho[j]<0){
    					int kcc=0;
    					while(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]!=dados.CC.rotuloAcop[iacop]) kcc++;
    					if(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]==dados.CC.rotuloAcop[iacop]){
    						malha.mlh3d[i].ccTambR[j]=dados.tParede[iacop];
    					}
    				}
    			}
    		}
    	}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

	ki=0.;
	cpi=0.;
	rhoi=0.;
	visci=0.;
	kontaTempo=0;
}

solverP3D::solverP3D():
		dados(),termolivre(1),matglob(1,1){
		nacop=0;
		//dutosMRT =0;
	    DGP3D=0;
		ki=0.;
		cpi=0.;
		rhoi=0.;
		visci=0.;
		itera=0;
		penalizaDt=0;
		kontaTempo=0;

	    rank=dados.rank;
	    precn=1;
	    //noZeroIlu=0;
	    //veclevILU=0;
	    //vecRelILU=0;
	    //veclmILU=0;
	    //vecfnzILU=0;

}

solverP3D::solverP3D(const solverP3D& vPoisson):
		dados(vPoisson.dados.entrada),termolivre(dados.nele),matglob(dados.nele,dados.noZero,dados.vecsra,dados.veclm,dados.vecfnz){
	nacop=vPoisson.nacop;
	itera=0;
	penalizaDt=0;
	//if(nacop>0){
		//dutosMRT = new DadosGeo [nacop];
		//for(int iacop=0; iacop<nacop; iacop++)dutosMRT[iacop]=vPoisson.dutosMRT[iacop];
	//}
	DGP3D=vPoisson.DGP3D;
	(*DGP3D).tempo=0.;

	rank=dados.rank;
    precn=4;
    if(rank<0)precn=1;
    matglob.colorido=dados.colorido;
    matglob.rank=rank;

    malha=malha3d(DGP3D,
    		dados.elementoO, dados.xcoor, dados.noEle, dados.tipo,
			dados.atributo, 4, dados.nele, dados.nno);

    /*noZeroIlu=vPoisson.noZeroIlu;
    veclevILU=new int [noZeroIlu];
    vecRelILU=new int [noZeroIlu];
    veclmILU=new int [noZeroIlu];
    vecfnzILU=new int [malha.nele+1];
    for(int i=0;i<noZeroIlu;i++){
    	veclevILU[i]=vPoisson.veclevILU[i];
    	vecRelILU[i]=vPoisson.vecRelILU[i];
    	veclmILU[i]=vPoisson.veclmILU[i];
    }
    for(int i=0;i<=malha.nele;i++){
    	vecfnzILU[i]=vPoisson.vecfnzILU[i];
    }*/

    for(int i=0;i<malha.nele;i++){
    	malha.mlh3d[i].CC=&dados.CC;
    	for(int j=0; j<dados.prop.parserie;j++){
    		double xc=malha.mlh3d[i].cel3D.centroideElem[0];
    		double yc=malha.mlh3d[i].cel3D.centroideElem[1];
    		if(malha.mlh3d[i].eleUNV.regiao==dados.prop.regiao[j]){
    			malha.mlh3d[i].cel3D.cond=dados.prop.cond[j];
    			malha.mlh3d[i].cel3D.cp=dados.prop.cp[j];
    			malha.mlh3d[i].cel3D.rho=dados.prop.rho[j];
    			break;
    		}
    	}
    	//for(int j=0; j<dados.CI.parserie;j++){
    		double xc=malha.mlh3d[i].cel3D.centroideElem[0];
    		double yc=malha.mlh3d[i].cel3D.centroideElem[1];
    		//if(malha.mlh3d[i].eleUNV.regiao==dados.CI.regiao[j]){
    			malha.mlh3d[i].cel3D.tempC=vPoisson.malha.mlh3d[i].cel3D.tempC;
    			malha.mlh3d[i].cel3D.tempC0=vPoisson.malha.mlh3d[i].cel3D.tempC0;
    			malha.mlh3d[i].cel3D.tempCI=vPoisson.malha.mlh3d[i].cel3D.tempCI;
    			//break;
    		//}
    	//}
    	for(int j=0; j<malha.mlh3d[i].cel3D.nvert;j++){
    		if(malha.mlh3d[i].kvizinho[j]<0){
    			malha.mlh3d[i].atualizaCC(j);
    		}
    	}
    }

    int indsra=0;
    for(int i=0;i<malha.nele;i++){
    	int dimNoZero=malha.mlh3d[i].cel3D.indVizCres.size();
    	//vecfnz[i]=malha.mlh3d[i].cel3D.indVizCres[0];
    	matglob.fnz[i]=indsra;
    	for(int j=0;j<dimNoZero;j++){
    		int ind=malha.mlh3d[i].cel3D.indVizCres[j];
    		matglob.sra[indsra+j]=malha.mlh3d[i].local.mx[0][j];
    		matglob.clm[indsra+j]=ind;
    	}
    	indsra+=dimNoZero;
    	termolivre[i]=malha.mlh3d[i].TL[0];
    }
    matglob.fnz[malha.nele]=dados.noZero;

    /*if(rank>0){
    	matglob.geraILUrank();
    	matglob.sraILU=new double[matglob.noZeroIlu];
    }
    if(matglob.colorido)matglob.geraILUcor();*/


    matglob.noZeroIlu=vPoisson.matglob.noZeroIlu;
    if(matglob.noZeroIlu>0){
    	matglob.veclmILU = new int[matglob.noZeroIlu];
    	matglob.veclevILU = new int[matglob.noZeroIlu];
    	matglob.vecRelILU = new int[matglob.noZeroIlu];
  	    for(int i=0;i<matglob.noZeroIlu;i++){
  	    	matglob.veclmILU[i] = vPoisson.matglob.veclmILU[i];
  	    	matglob.veclevILU[i] = vPoisson.matglob.veclevILU[i];
  	    	matglob.vecRelILU[i] = vPoisson.matglob.vecRelILU[i];
  	    }
  	    matglob.vecfnzILU = new int[malha.nele + 1];
  	    for(int i=0;i<malha.nele + 1;i++)matglob.vecfnzILU[i] = vPoisson.matglob.vecfnzILU[i];
  	    matglob.sraILU=new double[matglob.noZeroIlu];
    }
    else{
    	matglob.veclevILU=0;
    	matglob.vecRelILU=0;
    	matglob.veclmILU=0;
    	matglob.vecfnzILU=0;
    	matglob.sraILU=0;
    }

    matglob.color_forward=vPoisson.matglob.color_forward;
    matglob.color_backward=vPoisson.matglob.color_backward;

	//matglob.noZeroIlu=noZeroIlu;
	//matglob.rank=rank;
	//matglob.vecRelILU=vecRelILU;
	//matglob.vecfnzILU=vecfnzILU;
	//matglob.veclevILU=veclevILU;
	//matglob.veclmILU=veclmILU;
	//matglob.sraILU=new double[matglob.noZeroIlu];
	//matglob.colorido=dados.colorido;

    /*if(dados.colorido){
    	// Colorização para L (forward)
    	matglob.color_forward = vPoisson.matglob.color_forward;

    	// Colorização para U (backward)
    	matglob.color_backward = vPoisson.matglob.color_backward;
    }*/


    if(dados.acop==1){
    	for(int iacop=0;iacop<dados.CC.nAcop;iacop++){
    		dados.hE[iacop]=vPoisson.dados.hE[iacop];
    		dados.hI[iacop]=vPoisson.dados.hI[iacop];
    		dados.tInt[iacop]=vPoisson.dados.tInt[iacop];
    		/*dados.hE[iacop]=dados.transfer[iacop].hExt(0, dados.diamRef[iacop]);
    		dados.hI[iacop]=dados.transfer[iacop].hInt(0, dados.dutosMRT[iacop].a);
    		dados.condGlob[iacop]=dados.transfer[iacop].condParede();
    		dados.condLoc[iacop]=dados.transfer[iacop].condParedeLocal();
    		double multCond=1;
    		if(dados.soConvInt[iacop]==1)multCond=0.;
    	//tParede=
    		dados.qDesacop[iacop]=((2*M_PI-dados.angAcop)*(dados.tInt[iacop]-dados.tAmb))/
    				(multCond/dados.condGlob[iacop]+1./(0.5*dados.dutosMRT[iacop].a*dados.hI[iacop])+
    			1./(0.5*dados.dutosMRT[iacop].diamC[dados.dutosMRT[iacop].ncamadas-1]*dados.hE[iacop]));*/
    		dados.qAcop[iacop]=0.;
    	//por enquanto será retirrado este trecho. ATENCAO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    	/*for(int i=0; i<dados.CC.nRic; i++){
    		for(int j=0; j<dados.CC.ccRic[i].nserie; j++){
    			dados.CC.ccRic[i].valAmb[j]=dados.tAmb;
    			dados.CC.ccRic[i].hAmb[j]=dados.hE;
    		}
    	}*/
    		for(int i=0;i<malha.nele;i++){
    			for(int j=0; j<malha.mlh3d[i].cel3D.nvert;j++){
    				if(malha.mlh3d[i].kvizinho[j]<0){
    					int kcc=0;
    					while(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]!=dados.CC.rotuloAcop[iacop]) kcc++;
						if(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]==dados.CC.rotuloAcop[iacop]){
							malha.mlh3d[i].ccHR[j]=dados.hI[iacop];
						//qAcop+=malha.mlh3d[i].cel3D.sFaceMod[j]*(tInt-malha.mlh3d[i].tempF[j])/(1/condLoc+1./(hI));
							dados.qAcop[iacop]+=malha.mlh3d[i].cel3D.sFaceMod[j]*
								(dados.tInt[iacop]-malha.mlh3d[i].cel3D.tempC)/(1./(dados.hI[iacop]));
						}
    				}
    			}
    		}
    		dados.qTotal[iacop]=dados.qAcop[iacop];
    		dados.tParede[iacop]=dados.tInt[iacop];
    		for(int i=0;i<malha.nele;i++){
    			for(int j=0; j<malha.mlh3d[i].cel3D.nvert;j++){
    				if(malha.mlh3d[i].kvizinho[j]<0){
    					int kcc=0;
    					while(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]!=dados.CC.rotuloAcop[iacop]) kcc++;
    					if(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]==dados.CC.rotuloAcop[iacop]){
    						malha.mlh3d[i].ccTambR[j]=dados.tParede[iacop];
    					}
    				}
    			}
    		}
    	}

		///////////////////////////////////////////////////////////////////////////////////////////////////

    }
	ki=vPoisson.ki;
	cpi=vPoisson.cpi;
	rhoi=vPoisson.rhoi;
	visci=vPoisson.visci;
	kontaTempo=vPoisson.kontaTempo;
}

solverP3D& solverP3D::operator =(const solverP3D& vPoisson) {
	if (this != &vPoisson && vPoisson.dados.nele>0) {
		//if(nacop>0)delete[] dutosMRT;
		nacop=vPoisson.nacop;
		itera=0;
		penalizaDt=0;
		//if(nacop>0){
			//dutosMRT = new DadosGeo [nacop];
			//for(int iacop=0; iacop<nacop; iacop++)dutosMRT[iacop]=vPoisson.dutosMRT[iacop];
		//}

		/*if(rank>0 && noZeroIlu>0){
			delete[] veclevILU;
			delete[] vecRelILU;
			delete[] veclmILU;
			delete[] vecfnzILU;
		}*/

		DGP3D=vPoisson.DGP3D;
		dados=vPoisson.dados;
		termolivre=vPoisson.termolivre;
		matglob=vPoisson.matglob;

		rank=dados.rank;
	    precn=4;
	    if(rank<0)precn=1;
	    matglob.colorido=dados.colorido;
	    matglob.rank=rank;

	    (*DGP3D).tempo=0.;
	    if(malha.mlh3d.size()>0)malha.mlh3d.clear();
	    malha=malha3d(DGP3D,
	    		dados.elementoO, dados.xcoor, dados.noEle, dados.tipo,
				dados.atributo, 4, dados.nele, dados.nno);

	    /*if(rank>0){
	    	noZeroIlu=vPoisson.noZeroIlu;
	    	veclevILU=new int [noZeroIlu];
	    	vecRelILU=new int [noZeroIlu];
	    	veclmILU=new int [noZeroIlu];
	    	vecfnzILU=new int [malha.nele+1];
	    	for(int i=0;i<noZeroIlu;i++){
	    		veclevILU[i]=vPoisson.veclevILU[i];
	    		vecRelILU[i]=vPoisson.vecRelILU[i];
	    		veclmILU[i]=vPoisson.veclmILU[i];
	    	}
	    	for(int i=0;i<=malha.nele;i++){
	    		vecfnzILU[i]=vPoisson.vecfnzILU[i];
	    	}
	    }*/

	    for(int i=0;i<malha.nele;i++){
	    	malha.mlh3d[i].CC=&dados.CC;
	    	for(int j=0; j<dados.prop.parserie;j++){
	    		double xc=malha.mlh3d[i].cel3D.centroideElem[0];
	    		double yc=malha.mlh3d[i].cel3D.centroideElem[1];
	    		if(malha.mlh3d[i].eleUNV.regiao==dados.prop.regiao[j]){
	    			malha.mlh3d[i].cel3D.cond=dados.prop.cond[j];
	    			malha.mlh3d[i].cel3D.cp=dados.prop.cp[j];
	    			malha.mlh3d[i].cel3D.rho=dados.prop.rho[j];
	    			break;
	    		}
	    	}
	    	//for(int j=0; j<dados.CI.parserie;j++){
	    		double xc=malha.mlh3d[i].cel3D.centroideElem[0];
	    		double yc=malha.mlh3d[i].cel3D.centroideElem[1];
	    		//if(malha.mlh3d[i].eleUNV.regiao==dados.prop.regiao){
	    			malha.mlh3d[i].cel3D.tempC=vPoisson.malha.mlh3d[i].cel3D.tempC;
	    			malha.mlh3d[i].cel3D.tempC0=vPoisson.malha.mlh3d[i].cel3D.tempC0;
	    			malha.mlh3d[i].cel3D.tempCI=vPoisson.malha.mlh3d[i].cel3D.tempCI;
	    			//break;
	    		//}
	    	//}
	    	for(int j=0; j<malha.mlh3d[i].cel3D.nvert;j++){
	    		if(malha.mlh3d[i].kvizinho[j]<0){
	    			malha.mlh3d[i].atualizaCC(j);
	    		}
	    	}
	    }

	    int indsra=0;
	    for(int i=0;i<malha.nele;i++){

	    	int dimNoZero=malha.mlh3d[i].cel3D.indVizCres.size();
	    	//vecfnz[i]=malha.mlh3d[i].cel3D.indVizCres[0];
	    	matglob.fnz[i]=indsra;
	    	for(int j=0;j<dimNoZero;j++){
	    		int ind=malha.mlh3d[i].cel3D.indVizCres[j];
	    		matglob.sra[indsra+j]=malha.mlh3d[i].local.mx[0][j];
	    		matglob.clm[indsra+j]=ind;
	    	}
	    	indsra+=dimNoZero;
	    	termolivre[i]=malha.mlh3d[i].TL[0];
	    }
	    matglob.fnz[malha.nele]=dados.noZero;

	    if(matglob.noZeroIlu>0){
	  	    for(int i=0;i<matglob.noZeroIlu;i++){
	  	    	matglob.veclmILU[i] = vPoisson.matglob.veclmILU[i];
	  	    	matglob.veclevILU[i] = vPoisson.matglob.veclevILU[i];
	  	    	matglob.vecRelILU[i] = vPoisson.matglob.vecRelILU[i];
	  	    }
	  	    for(int i=0;i<malha.nele + 1;i++)matglob.vecfnzILU[i] = vPoisson.matglob.vecfnzILU[i];
	    }
	    else{
	    	matglob.veclevILU=0;
	    	matglob.vecRelILU=0;
	    	matglob.veclmILU=0;
	    	matglob.vecfnzILU=0;
	    	matglob.sraILU=0;
	    }

	    matglob.color_forward=vPoisson.matglob.color_forward;
	    matglob.color_backward=vPoisson.matglob.color_backward;

	    /*if(rank>0){
	    	matglob.geraILUrank();
	    	matglob.sraILU=new double[matglob.noZeroIlu];
	    }
	    if(matglob.colorido)matglob.geraILUcor();*/

		/*matglob.colorido=dados.colorido;

	    if(dados.colorido){
	    	// Colorização para L (forward)
	    	matglob.color_forward = vPoisson.matglob.color_forward;

	    	// Colorização para U (backward)
	    	matglob.color_backward = vPoisson.matglob.color_backward;
	    }*/


	    if(dados.acop==1){
	    	for(int iacop=0;iacop<dados.CC.nAcop;iacop++){
	    		dados.hE[iacop]=vPoisson.dados.hE[iacop];
	    		dados.hI[iacop]=vPoisson.dados.hI[iacop];
	    		dados.tInt[iacop]=vPoisson.dados.tInt[iacop];
	    		/*dados.hE[iacop]=dados.transfer[iacop].hExt(0, dados.diamRef[iacop]);
	    		dados.hI[iacop]=dados.transfer[iacop].hInt(0, dados.dutosMRT[iacop].a);
	    		dados.condGlob[iacop]=dados.transfer[iacop].condParede();
	    		dados.condLoc[iacop]=dados.transfer[iacop].condParedeLocal();
	    		double multCond=1;
	    		if(dados.soConvInt[iacop]==1)multCond=0.;
	    	//tParede=
	    		dados.qDesacop[iacop]=((2*M_PI-dados.angAcop)*(dados.tInt[iacop]-dados.tAmb))/
	    				(1./dados.condGlob[iacop]+1./(0.5*dados.dutosMRT[iacop].a*dados.hI[iacop])+
	    				1./(0.5*dados.dutosMRT[iacop].diamC[dados.dutosMRT[iacop].ncamadas-1]*dados.hE[iacop]));*/
	    		dados.qAcop[iacop]=0.;
	    	//por enquanto será retirrado este trecho. ATENCAO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	    	/*for(int i=0; i<dados.CC.nRic; i++){
	    		for(int j=0; j<dados.CC.ccRic[i].nserie; j++){
	    			dados.CC.ccRic[i].valAmb[j]=dados.tAmb;
	    			dados.CC.ccRic[i].hAmb[j]=dados.hE;
	    		}
	    	}*/
	    		for(int i=0;i<malha.nele;i++){
	    			for(int j=0; j<malha.mlh3d[i].cel3D.nvert;j++){
	    				if(malha.mlh3d[i].kvizinho[j]<0){
	    					int kcc=0;
	    					while(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]!=dados.CC.rotuloAcop[iacop]) kcc++;
	    					if(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]==dados.CC.rotuloAcop[iacop]){
	    						malha.mlh3d[i].ccHR[j]=dados.hI[iacop];
							//qAcop+=malha.mlh3d[i].cel3D.sFaceMod[j]*(tInt-malha.mlh3d[i].tempF[j])/(1/condLoc+1./(hI));
	    						dados.qAcop[iacop]+=malha.mlh3d[i].cel3D.sFaceMod[j]*
	    								(dados.tInt[iacop]-malha.mlh3d[i].cel3D.tempC)/
										(1./(dados.hI[iacop]));
	    					}
	    				}
	    			}
	    		}
	    		dados.qTotal[iacop]=dados.qAcop[iacop];
	    		dados.tParede[iacop]=dados.tInt[iacop];
	    		for(int i=0;i<malha.nele;i++){
	    			for(int j=0; j<malha.mlh3d[i].cel3D.nvert;j++){
	    				if(malha.mlh3d[i].kvizinho[j]<0){
	    					int kcc=0;
	    					while(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]!=dados.CC.rotuloAcop[iacop]) kcc++;
	    					if(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]==dados.CC.rotuloAcop[iacop]){
	    						malha.mlh3d[i].ccTambR[j]=dados.tParede[iacop];
	    					}
	    				}
	    			}
	    		}
	    	}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	    }

		ki=vPoisson.ki;
		cpi=vPoisson.cpi;
		rhoi=vPoisson.rhoi;
		visci=vPoisson.visci;
		kontaTempo=vPoisson.kontaTempo;
	}

	return *this;
}

void solverP3D::indrazT(int& ind, double& raz) {

	for (int i = 0; i <= dados.temp.parserie - 1; i++) {
		if (i < dados.temp.parserie - 1) {
			if ((*DGP3D).tempo >= dados.temp.tempo[i] && (*DGP3D).tempo < dados.temp.tempo[i + 1]) {
				ind = i;
				raz = 1
						- ((*DGP3D).tempo - dados.temp.tempo[i])
								/ (dados.temp.tempo[i + 1] - dados.temp.tempo[i]);
				break;
			}
			//else if(i==parserie-2){
			//ind=i+1;
			//raz=1;
			//}
		} else if (i == dados.temp.parserie - 1) {
			ind = i;
			raz = 1;
		}
	}
}

int solverP3D::permanentePoisson(){
    double norma=1e5;
    int itera=0;
    int indsra;
   // Vcr<double> termolivre(malha.nele);
    int diverge=0;
    double norma0=norma;
    while((norma>1e-5 || itera<5) && diverge==0){
    	for(int i=0; i<malha.nele;i++){
    		for(int j=0; j<malha.mlh3d[i].cel3D.dim;j++){
    			malha.mlh3d[i].cel3D.gradGreenTI[j]=malha.mlh3d[i].cel3D.gradGreenT[j];
    		}
    	}
    	for(int i=0; i<malha.nele;i++){
    		malha.mlh3d[i].calcGradGreen();
    	}
    	for(int i=0; i<malha.nele;i++){
    		malha.mlh3d[i].GeraLocal(dados.temp.relax);
    	}
    	for(int i=0;i<malha.nele;i++)
    	    		malha.mlh3d[i].cel3D.tempC0=malha.mlh3d[i].cel3D.tempCI=malha.mlh3d[i].cel3D.tempC;
    	indsra=0;
    	for(int i=0;i<malha.nele;i++){
    		int dimNoZero=malha.mlh3d[i].cel3D.indVizCres.size();
    		for(int j=0;j<dimNoZero;j++)
    			matglob.sra[indsra+j]=malha.mlh3d[i].local.mx[0][j];
    		indsra+=dimNoZero;
    		termolivre[i]=malha.mlh3d[i].TL[0];
    	}
    	double eps=0.00001;
    	int iter=malha.nele;
    	int retorna;
    	for(int i=0;i<malha.nele;i++)malha.vecSolv[i]=malha.mlh3d[i].cel3D.tempC;
    	retorna=matglob.GMRES(malha.vecSolv,termolivre,eps,iter,precn,40+0*malha.nele-1);
    	//retorna=matglob.CG(malha.vecSolv,termolivre,eps,iter,1);
    	//if(retorna==1)break;
    	/////////////////////////////////////////////////////////////////////////////////////////////////
    	///////////////////////////////////////////////////////////////////////////////////////////////////

    	norma=0.;
    	for(int i=0;i<malha.nele;i++){
    		//malha.mlh3d[i].cel3D.tempC0=malha.mlh3d[i].cel3D.tempCI=malha.mlh3d[i].cel3D.tempC;
    		malha.mlh3d[i].cel3D.tempC=malha.vecSolv[i];
    		norma+=fabsl(malha.mlh3d[i].cel3D.tempC-malha.mlh3d[i].cel3D.tempC0);
    	}
    	norma/=malha.nele;
    	if(norma>=norma0)diverge=1;
    	else norma0=norma;

	    if(dados.acop==1){
	    	//hE=transfer.hExt();
	    	for(int iacop=0;iacop<dados.CC.nAcop;iacop++){
	    		/*dados.transfer[iacop].Tint=dados.tInt[iacop];
	    		dados.transfer[iacop].kint=dados.fluc.CondLiq(dados.pInt, dados.tInt[iacop]);
	    		dados.transfer[iacop].cpint=dados.fluc.CalorLiq(dados.pInt, dados.tInt[iacop]);
	    		dados.transfer[iacop].rhoint=dados.fluc.MasEspFlu(dados.pInt, dados.tInt[iacop]);
	    		dados.transfer[iacop].viscint=dados.fluc.VisFlu(dados.pInt, dados.tInt[iacop]);
	    		dados.hI[iacop]=dados.transfer[iacop].hInt(0, dados.dutosMRT[iacop].a);
	    		double multCond=1;
	    		if(dados.soConvInt[iacop]==1)multCond=0.;
	    		dados.qDesacop[iacop]=((2*M_PI-dados.angAcop)*(dados.tInt[iacop]-dados.tAmb))*
	    			dados.corte[iacop].comp/(multCond/dados.condGlob[iacop]+1./(0.5*dados.dutosMRT[iacop].a*dados.hI[iacop])+
	    			  1./(0.5*dados.dutosMRT[iacop].diamC[dados.dutosMRT[iacop].ncamadas-1]*dados.hE[iacop]));*/
	    		dados.qAcop[iacop]=0.;
	    		for(int i=0;i<malha.nele;i++){
	    			for(int j=0; j<malha.mlh3d[i].cel3D.nvert;j++){
	    				if(malha.mlh3d[i].kvizinho[j]<0){
	    					int kcc=0;
	    					while(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]!=dados.CC.rotuloAcop[iacop]) kcc++;
	    					if(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]==dados.CC.rotuloAcop[iacop]){
	    						malha.mlh3d[i].ccHR[j]=dados.hI[iacop];
	    						dados.qAcop[iacop]+=malha.mlh3d[i].cel3D.sFaceMod[j]*
	    								(dados.tInt[iacop]-malha.mlh3d[i].tempF[j])/(1./(dados.hI[iacop]));
	    					}
	    				}
	    			}
	    		}
	    		dados.qTotal[iacop]=dados.qAcop[iacop];
	    		dados.tParede[iacop]=dados.tInt[iacop];
	    		for(int i=0;i<malha.nele;i++){
	    			for(int j=0; j<malha.mlh3d[i].cel3D.nvert;j++){
	    				if(malha.mlh3d[i].kvizinho[j]<0){
	    					int kcc=0;
	    					while(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]!=dados.CC.rotuloAcop[iacop]) kcc++;
	    					if(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]==dados.CC.rotuloAcop[iacop]){
	    						malha.mlh3d[i].ccTambR[j]=dados.tParede[iacop];
	    					}
	    				}
	    			}
	    		}
	    	}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	    }
	    itera++;
    }
    malha.imprime(0.);
    return diverge;
}

void solverP3D::inicializaPermanentePoisson(){
    //for(int i=0;i<malha.nele;i++){
    	//malha.mlh3d[i].perm=1;
    	//malha.mlh3d[i].trans=0;
    //}

    for(int iacop=0;iacop<dados.CC.nAcop;iacop++){
    	dados.tInt0[iacop]=dados.tInt[iacop];
    }
}

void solverP3D::inicializaTransientePoisson(){
    //if(dados.temp.perm==1 && dados.temp.trans==1){
    	//for(int i=0;i<malha.nele;i++){
    		//malha.mlh3d[i].perm=0;
    		//malha.mlh3d[i].trans=1;
    	//}
    //}

	//for(int i=0;i<malha.nele;i++){
		//malha.mlh3d[i].perm=0;
		//malha.mlh3d[i].trans=1;
	//}
	kontaTempo=0;
    for(int iacop=0;iacop<dados.CC.nAcop;iacop++){
    	dados.tInt0[iacop]=dados.tInt[iacop];
    }
}

double solverP3D::defineDeltPoisson(){
    int ind=0;
	double raz;
	double delt;

	indrazT(ind, raz);
	double inf = dados.temp.dtmax[ind];
	double sup;
	if (ind < dados.temp.parserie - 1){
		sup = dados.temp.dtmax[ind + 1];
		delt = (inf * raz + (1 - raz) * sup);
	}
	else
		delt = dados.temp.dtmax[ind];
	return delt;
}

void solverP3D::transientePoisson(double vdelt){
	double delt=vdelt;
	double norma;
	//int itera;
	int indsra;
	(*DGP3D).tempo=(*DGP3D).lixo5;
	//if(dados.temp.trans==1){
		//while((*DGP3D).tempo<=dados.temp.tmax){
			norma=1e5;
			/*indrazT(ind, raz);
			double inf = dados.temp.dtmax[ind];
			double sup;
			if (ind < dados.temp.parserie - 1){
				sup = dados.temp.dtmax[ind + 1];
				delt = (inf * raz + (1 - raz) * sup);
			}
			else
				delt = dados.temp.dtmax[ind];*/
			for(int i=0; i<malha.nele;i++)malha.mlh3d[i].dt=delt;
			for(int i=0;i<malha.nele;i++){
		    	for(int j=0; j<malha.mlh3d[i].cel3D.nvert;j++){
		    		if(malha.mlh3d[i].kvizinho[j]<0){
		    			malha.mlh3d[i].atualizaCC(j);
		    		}
		    	}
			}
			itera=0;
			while(norma>1.e-20 || itera<5){
                #pragma omp parallel for num_threads((*DGP3D).ntrd3DP)
		    	for(int i=0; i<malha.nele;i++){
		    		for(int j=0; j<malha.mlh3d[i].cel3D.dim;j++){
		    			malha.mlh3d[i].cel3D.gradGreenTI[j]=malha.mlh3d[i].cel3D.gradGreenT[j];
		    		}
		    	}
                #pragma omp parallel for num_threads((*DGP3D).ntrd3DP)
				for(int i=0; i<malha.nele;i++){
					malha.mlh3d[i].calcGradGreen();
				}
                #pragma omp parallel for num_threads((*DGP3D).ntrd3DP)
				for(int i=0; i<malha.nele;i++){
					malha.mlh3d[i].GeraLocal(dados.temp.relax);
				}
				indsra=0;
				for(int i=0;i<malha.nele;i++){
					int dimNoZero=malha.mlh3d[i].cel3D.indVizCres.size();
					for(int j=0;j<dimNoZero;j++)
						matglob.sra[indsra+j]=malha.mlh3d[i].local.mx[0][j];
					indsra+=dimNoZero;
					termolivre[i]=malha.mlh3d[i].TL[0];
				}
				double eps=0.0000001;
				int iter=malha.nele;
				int retorna;
				for(int i=0;i<malha.nele;i++){
					if(i==16220){
						int para;
						para=0;
					}
					malha.vecSolv[i]=malha.mlh3d[i].cel3D.tempC;
				}
				//int precn=4;
				//if(itera>2 && norma>1.e-20)precn=3;
				if(dados.solverMat==0)retorna=matglob.GMRES(malha.vecSolv,termolivre,eps,iter,precn,40+0*(malha.nele-1));
				else if(dados.solverMat==1)retorna=matglob.FGMRES(malha.vecSolv,termolivre,eps,iter,precn,40+0*(malha.nele-1));
				else retorna=matglob.BiCGStab(malha.vecSolv,termolivre,eps,iter,precn);
				//retorna=matglob.CG(malha.vecSolv,termolivre,eps,iter,1);
				//if(retorna==1)break;
				norma=0.;
                #pragma omp parallel for num_threads((*DGP3D).ntrd3DP)
				for(int i=0;i<malha.nele;i++){
					malha.mlh3d[i].cel3D.tempCI=malha.mlh3d[i].cel3D.tempC;
					malha.mlh3d[i].cel3D.tempC=malha.vecSolv[i];
					norma+=fabsl(malha.mlh3d[i].cel3D.tempC-malha.mlh3d[i].cel3D.tempCI);
					if(fabsl(malha.mlh3d[i].cel3D.tempC-malha.mlh3d[i].cel3D.tempCI)>1){
						int para;
						para=0;
					}
				}
				norma/=malha.nele;

				/*for(int i=0;i<malha.nele;i++){
					if(malha.mlh3d[i].eleUNV.RAspec>0*1.+1*5.){
						double vol=0;
						double substitua=0;
						for(int j=0;j<malha.mlh3d[i].cel3D.nvert;j++){
							int viz=malha.mlh3d[i].kvizinho[j];
							if(viz>=0){
								substitua+=(*(malha.mlh3d[i].vizinho[j])).tempC*(*(malha.mlh3d[i].vizinho[j])).vElem;
								vol+=(*(malha.mlh3d[i].vizinho[j])).vElem;
							}
						}
						if(vol>0){
							malha.mlh3d[i].cel3D.tempC=substitua/vol;
						}
					}
				}*/

			    if(dados.acop==1){
			    	//hE=transfer.hExt();
			    	for(int iacop=0;iacop<dados.CC.nAcop;iacop++){
			    		/*dados.tInt[iacop]=dados.tInt0[iacop]-
			    				dados.qTotal[iacop]*delt/(dados.transfer[iacop].cpint*dados.transfer[iacop].rhoint*
			    						dados.dutosMRT[iacop].area*dados.corte[iacop].comp);
			    		dados.transfer[iacop].kint=ki=dados.fluc.CondLiq(dados.pInt, dados.tInt[iacop]);
			    		dados.transfer[iacop].cpint=cpi=dados.fluc.CalorLiq(dados.pInt, dados.tInt[iacop]);
			    		dados.transfer[iacop].rhoint=rhoi=dados.fluc.MasEspFlu(dados.pInt, dados.tInt[iacop]);
			    		dados.transfer[iacop].viscint=visci=dados.fluc.VisFlu(dados.pInt, dados.tInt[iacop]);
			    		dados.transfer[iacop].Vint=0.;
			    		dados.hI[iacop]=dados.transfer[iacop].hInt(0, dados.dutosMRT[iacop].a);
			    		double multCond=1;
			    		if(dados.soConvInt[iacop]==1)multCond=0.;*/
			    		/*dados.qDesacop[iacop]=((2*M_PI-dados.angAcop)*
			    			(dados.tInt[iacop]-dados.tAmb)*dados.corte[iacop].comp)/(multCond/dados.condGlob[iacop]+
			    					1./(0.5*dados.dutosMRT[iacop].a*dados.hI[iacop])+
			    			  1./(0.5*dados.dutosMRT[iacop].diamC[dados.dutosMRT[iacop].ncamadas-1]*dados.hE[iacop]));*/
			    		dados.qAcop[iacop]=0.;
			    		for(int i=0;i<malha.nele;i++){
			    			for(int j=0; j<malha.mlh3d[i].cel3D.nvert;j++){
			    				if(malha.mlh3d[i].kvizinho[j]<0){
			    					int kcc=0;
			    					while(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]!=dados.CC.rotuloAcop[iacop]) kcc++;
			    					if(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]==dados.CC.rotuloAcop[iacop]){
			    						malha.mlh3d[i].ccHR[j]=dados.hI[iacop];
			    						dados.qAcop[iacop]+=malha.mlh3d[i].cel3D.sFaceMod[j]*
			    								(dados.tInt[iacop]-malha.mlh3d[i].tempF[j])/
											(1./(dados.hI[iacop]));
			    					}
			    				}
			    			}
			    		}
			    		dados.qTotal[iacop]=dados.qAcop[iacop];
			    		dados.tParede[iacop]=dados.tInt[iacop];
			    		for(int i=0;i<malha.nele;i++){
			    			for(int j=0; j<malha.mlh3d[i].cel3D.nvert;j++){
			    				if(malha.mlh3d[i].kvizinho[j]<0){
			    					int kcc=0;
			    					while(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]!=dados.CC.rotuloAcop[iacop]) kcc++;
			    					if(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]==dados.CC.rotuloAcop[iacop]){
			    						malha.mlh3d[i].ccTambR[j]=dados.tParede[iacop];
			    					}
			    				}
			    			}
			    		}
			    	}
			    	/*for(int idir=0;idir<dados.CC.nDiri;idir++){
			    		for(int i=0;i<malha.nele;i++){
			    			for(int j=0; j<malha.mlh3d[i].cel3D.nvert;j++){
			    				if(malha.mlh3d[i].kvizinho[j]<0){
			    					int kcc=0;
			    					while(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]!=dados.CC.ccRic[idir].rotulo) kcc++;
				    				if(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]==dados.CC.ccRic[idir].rotulo){
				    					malha.mlh3d[i].CC.ccRic[idir].valAmb[0]=dados.CC.ccRic[idir].valAmb[0];
				    				}
			    				}
			    			}
			    		}
			    	}*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			    }
			    itera++;

			}
			if(penalizaDt>0)penalizaDt--;

			cout << "tempo= " << (*DGP3D).tempo<<" norma= "<< norma<<" iteracoes= "<<itera;
			cout << endl;

			//(*DGP3D).tempo+=delt;

			if (((*DGP3D).tempo > 1e-15 && (*DGP3D).tempo <= dados.temp.tempoImp[kontaTempo] && (*DGP3D).tempo + delt >= dados.temp.tempoImp[kontaTempo])) {
				malha.imprime((*DGP3D).tempo);
				kontaTempo++;
				if (kontaTempo >= dados.temp.parserieImp) kontaTempo--;
			}
		//}
	//}
}

void solverP3D::renova(){
    for(int iacop=0;iacop<dados.CC.nAcop;iacop++){
    	dados.tInt0[iacop]=dados.tInt[iacop];
    }
	for(int i=0;i<malha.nele;i++){
		malha.mlh3d[i].cel3D.tempC0=malha.mlh3d[i].cel3D.tempC;
		//for(int j=0;j<malha.mlh3d[i].cel3D.dim;j++)
			//malha.mlh3d[i].cel3D.gradGreenT0[j]=malha.mlh3d[i].cel3D.gradGreenT[j];
		//malha.mlh3d[i].cel3D.tempC=malha.vecSolv[i];
	}
}

void solverP3D::transientePoissonDummy(double vdelt,int konta){
	double delt=vdelt;
	double norma;
	int itera;
	int indsra;
	//if(dados.temp.trans==1){
		//while((*DGP3D).tempo<=dados.temp.tmax){
			norma=1e5;
			/*indrazT(ind, raz);
			double inf = dados.temp.dtmax[ind];
			double sup;
			if (ind < dados.temp.parserie - 1){
				sup = dados.temp.dtmax[ind + 1];
				delt = (inf * raz + (1 - raz) * sup);
			}
			else
				delt = dados.temp.dtmax[ind];*/
			for(int i=0; i<malha.nele;i++)malha.mlh3d[i].dt=delt;
			for(int i=0;i<malha.nele;i++){
		    	for(int j=0; j<malha.mlh3d[i].cel3D.nvert;j++){
		    		if(malha.mlh3d[i].kvizinho[j]<0){
		    			malha.mlh3d[i].atualizaCC(j);
		    		}
		    	}
			}
			itera=0;
			while(norma>1.e-20 || itera<5){
			//while( itera<1){
				#pragma omp parallel for num_threads((*DGP3D).ntrd3DP)
                //#pragma omp parallel for num_threads((*vg1dSP).ntrd)
		    	for(int i=0; i<malha.nele;i++){
		    		for(int j=0; j<malha.mlh3d[i].cel3D.dim;j++){
		    			malha.mlh3d[i].cel3D.gradGreenTI[j]=malha.mlh3d[i].cel3D.gradGreenT[j];
		    		}
		    	}
				#pragma omp parallel for num_threads((*DGP3D).ntrd3DP)
		    	//#pragma omp parallel for num_threads((*vg1dSP).ntrd)
				for(int i=0; i<malha.nele;i++) malha.mlh3d[i].calcGradGreen();
				#pragma omp parallel for num_threads((*DGP3D).ntrd3DP)
				//#pragma omp parallel for num_threads((*vg1dSP).ntrd)
				for(int i=0; i<malha.nele;i++) malha.mlh3d[i].GeraLocal(1.);
				indsra=0;
				for(int i=0;i<malha.nele;i++){
					int dimNoZero=malha.mlh3d[i].cel3D.indVizCres.size();
					for(int j=0;j<dimNoZero;j++)
						matglob.sra[indsra+j]=malha.mlh3d[i].local.mx[0][j];
					indsra+=dimNoZero;
					termolivre[i]=malha.mlh3d[i].TL[0];
				}
				double eps=0.0000001;
				int iter=malha.nele;
				int retorna;
				for(int i=0;i<malha.nele;i++)malha.vecSolv[i]=malha.mlh3d[i].cel3D.tempC;
				//retorna=matglob.GMRES(malha.vecSolv,termolivre,eps,iter,precn,40+0*malha.nele-1);
				if(dados.solverMat==0)retorna=matglob.GMRES(malha.vecSolv,termolivre,eps,iter,precn,40+0*(malha.nele-1));
				else if(dados.solverMat==1)retorna=matglob.FGMRES(malha.vecSolv,termolivre,eps,iter,precn,40+0*(malha.nele-1));
				else retorna=matglob.BiCGStab(malha.vecSolv,termolivre,eps,iter,precn);
				//if(retorna==1)break;
				norma=0.;
				for(int i=0;i<malha.nele;i++){
					malha.mlh3d[i].cel3D.tempCI=malha.mlh3d[i].cel3D.tempC;
					malha.mlh3d[i].cel3D.tempC=malha.vecSolv[i];
					norma+=fabsl(malha.mlh3d[i].cel3D.tempC-malha.mlh3d[i].cel3D.tempCI);
				}
				norma/=malha.nele;

			    if(dados.acop==1){
			    	//hE=transfer.hExt();
			    	for(int iacop=0;iacop<dados.CC.nAcop;iacop++){
			    		dados.qAcop[iacop]=0.;
			    		for(int i=0;i<malha.nele;i++){
			    			for(int j=0; j<malha.mlh3d[i].cel3D.nvert;j++){
			    				if(malha.mlh3d[i].kvizinho[j]<0){
			    					int kcc=0;
			    					while(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]!=dados.CC.rotuloAcop[iacop]) kcc++;
			    					if(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]==dados.CC.rotuloAcop[iacop]){
			    						malha.mlh3d[i].ccHR[j]=dados.hI[iacop];
			    						dados.qAcop[iacop]+=malha.mlh3d[i].cel3D.sFaceMod[j]*(dados.tInt[iacop]-malha.mlh3d[i].tempF[j])/
											(1./(dados.hI[iacop]));
			    					}
			    				}
			    			}
			    		}
			    		dados.qTotal[iacop]=dados.qAcop[iacop];
			    		dados.tParede[iacop]=dados.tInt[iacop];
			    		for(int i=0;i<malha.nele;i++){
			    			for(int j=0; j<malha.mlh3d[i].cel3D.nvert;j++){
			    				if(malha.mlh3d[i].kvizinho[j]<0){
			    					int kcc=0;
			    					while(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]!=dados.CC.rotuloAcop[iacop]) kcc++;
			    					if(kcc<1 && malha.mlh3d[i].cel3D.ccFace[j]==dados.CC.rotuloAcop[iacop]){
			    						malha.mlh3d[i].ccTambR[j]=dados.tParede[iacop];
			    					}
			    				}
			    			}
			    		}
			    	}
					///////////////////////////////////////////////////////////////////////////////////////////

			    }
			    itera++;

			}
		    for(int iacop=0;iacop<dados.CC.nAcop;iacop++){
		    	dados.tInt0[iacop]=dados.tInt[iacop];
		    }
			for(int i=0;i<malha.nele;i++){
				malha.mlh3d[i].cel3D.tempC0=malha.mlh3d[i].cel3D.tempC;
				//malha.mlh3d[i].cel3D.tempC=malha.vecSolv[i];
			}

			cout << "kontatempo= " << konta<<" norma= "<< norma<<" iteracoes= "<<itera<<" deltempo= "<<delt;
			cout << endl;

			//(*DGP3D).tempo+=delt;

			//if (((*DGP3D).tempo > 1e-15 && (*DGP3D).tempo <= dados.temp.(*DGP3D).tempoImp[konta(*DGP3D).tempo] && (*DGP3D).tempo + delt >= dados.temp.(*DGP3D).tempoImp[konta(*DGP3D).tempo])) {
				//malha.imprime((*DGP3D).tempo);
			/////////////////////////////////////////////////////////////////////////////
				//imprimeMalhaRegular();
			////////////////////////////////////////////////////////////////////////////
				//konta(*DGP3D).tempo++;
				//if (konta(*DGP3D).tempo >= dados.temp.parserieImp) konta(*DGP3D).tempo--;
			//}
		//}
	//}
}

void solverP3D::FeiticoDoTempo(){
    for(int iacop=0;iacop<dados.CC.nAcop;iacop++){
    	dados.tInt0[iacop]=dados.tInt[iacop];
    }
	for(int i=0;i<malha.nele;i++){
		malha.mlh3d[i].cel3D.tempC=malha.mlh3d[i].cel3D.tempC0;
		//for(int j=0;j<malha.mlh3d[i].cel3D.dim;j++)
			//malha.mlh3d[i].cel3D.gradGreenT[j]=malha.mlh3d[i].cel3D.gradGreenT0[j];
	}
}
