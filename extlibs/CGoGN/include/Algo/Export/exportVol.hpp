/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include "Topology/generic/attributeHandler.h"
#include "Topology/generic/autoAttributeHandler.h"
#include "Topology/generic/traversorCell.h"
#include "Topology/generic/traversor2.h"
#include "Topology/generic/cellmarker.h"
#include "Algo/Import/importFileTypes.h"

namespace CGoGN
{

namespace Algo
{

namespace Volume
{

namespace Export
{

inline std::string truncFloatTO8(float f)
{
	std::stringstream ss;
	ss << f;
	std::string res = ss.str();

	size_t expo = res.find('e');
	if (expo != std::string::npos)
	{
		if ( res[expo+2] == '0')
			return res.substr(0,6) + res[expo+1] + res[expo+3];

		return res.substr(0,5) + res.substr(expo+1);
	}
	return res.substr(0,8);
}

template <typename PFP>
bool exportNAS(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const char* filename)
{
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;

	// open file
	std::ofstream fout ;
	fout.open(filename, std::ios::out) ;

	if (!fout.good())
	{
		CGoGNerr << "Unable to open file " << filename << CGoGNendl ;
		return false ;
	}

	VertexAutoAttribute<unsigned int> indices(map,"indices_vert");


	fout << "$$ ---------------------------------------------------------------------------- $"<< std::endl;
	fout << "$$      NASTRAN MEsh File Generated by CGoGN (ICube/IGG)                        $"<< std::endl;
	fout << "$$ ---------------------------------------------------------------------------- $"<< std::endl;
	fout << "CEND" << std::endl;;
	fout << "BEGIN BULK" << std::endl;
	fout << "$$ ---------------------------------------------------------------------------- $"<< std::endl;
	fout << "$$      Vertices position                                                       $"<< std::endl;
	fout << "$$ ---------------------------------------------------------------------------- $"<< std::endl;
	unsigned int count=1;
	for (unsigned int i = position.begin(); i != position.end(); position.next(i))
	{
		const VEC3& P = position[i];
		fout << "GRID    ";
		fout << std::right;
		fout.width(8);
		fout << count;
		fout << "        ";
		fout << std::left;
		fout << std::setw(8)<<truncFloatTO8(P[0]) << std::setw(8)<<truncFloatTO8(P[1]) << std::setw(8)<<truncFloatTO8(P[2]) << std::endl;
		indices[i] = count++;
	}


	std::vector<unsigned int> hexa;
	std::vector<unsigned int> tetra;
	hexa.reserve(2048);
	tetra.reserve(2048);

	TraversorW<MAP> trav(map) ;
	for(Dart d = trav.begin(); d != trav.end(); d = trav.next())
	{
		unsigned int degree = 0 ;

		Traversor3WV<typename PFP::MAP> twv(map, d) ;
		for(Dart it = twv.begin(); it != twv.end(); it = twv.next())
		{
			degree++;
		}

		if (degree == 8)
		{
			//CAS HEXAEDRIQUE (ordre 2 quad superposes, le premier en CCW)
			Dart e = d;
			Dart f = map.template phi<21121>(d);
			hexa.push_back(indices[f]);
			f = map.phi_1(f);
			hexa.push_back(indices[f]);
			f = map.phi_1(f);
			hexa.push_back(indices[f]);
			f = map.phi_1(f);
			hexa.push_back(indices[f]);
			hexa.push_back(indices[e]);
			e = map.phi1(e);
			hexa.push_back(indices[e]);
			e = map.phi1(e);
			hexa.push_back(indices[e]);
			e = map.phi1(e);
			hexa.push_back(indices[e]);
		}

		if (degree == 4)
		{
			//CAS TETRAEDRIQUE
			Dart e = d;
			tetra.push_back(indices[e]);
			e = map.phi1(e);
			tetra.push_back(indices[e]);
			e = map.phi1(e);
			tetra.push_back(indices[e]);
			e = map.template phi<211>(e);
			tetra.push_back(indices[e]);
		}
	}

	unsigned int countCell=1;
	unsigned int nbhexa = hexa.size()/8;
	unsigned int nbtetra = tetra.size()/4;

	fout << std::right;
	if (nbhexa!=0)
	{
		fout << "$$ ---------------------------------------------------------------------------- $"<< std::endl;
		fout << "$$      Hexa indices                                                            $"<< std::endl;
		fout << "$$ ---------------------------------------------------------------------------- $"<< std::endl;
		for (unsigned int i=0; i<nbhexa; ++i)
		{
			fout << "CHEXA   ";
			fout << std::setw(8) << countCell++ << std::setw(8)<< 0;
			fout <<  std::setw(8) << hexa[8*i] <<  std::setw(8) << hexa[8*i+1] <<  std::setw(8) << hexa[8*i+2];
			fout <<  std::setw(8) << hexa[8*i+3] <<  std::setw(8) << hexa[8*i+4] <<  std::setw(8) << hexa[8*i+5] << "+"<< std::endl;
			fout << "+       " <<  std::setw(8) << hexa[8*i+6] <<  std::setw(8) << hexa[8*i+7] << std::endl;
		}
	}

	if (nbtetra != 0)
	{
		fout << "$$ ---------------------------------------------------------------------------- $"<< std::endl;
		fout << "$$      Tetra indices                                                           $"<< std::endl;
		fout << "$$ ---------------------------------------------------------------------------- $"<< std::endl;
		for (unsigned int i=0; i<nbtetra; ++i)
		{
			fout << "CTETRA  ";
			fout << std::setw(8) << countCell++ << std::setw(8)<< 0;
			fout << std::setw(8) << tetra[4*i] << std::setw(8) << tetra[4*i+1] << std::setw(8) << tetra[4*i+2] << std::setw(8) << tetra[4*i+3] << std::endl;
		}
	}

	fout << "ENDDATA" << std::endl;

	fout.close();
	return true;
}



template <typename PFP>
bool exportVTU(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const char* filename)
{
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;

	// open file
	std::ofstream fout ;
	fout.open(filename, std::ios::out) ;

	if (!fout.good())
	{
		CGoGNerr << "Unable to open file " << filename << CGoGNendl ;
		return false ;
	}

	VertexAutoAttribute<unsigned int> indices(map,"indices_vert");

	unsigned int count=0;
	for (unsigned int i = position.begin(); i != position.end(); position.next(i))
	{
		indices[i] = count++;
	}


	std::vector<unsigned int> hexa;
	std::vector<unsigned int> tetra;
	hexa.reserve(2048);
	tetra.reserve(2048);

	TraversorW<MAP> trav(map) ;
	for(Dart d = trav.begin(); d != trav.end(); d = trav.next())
	{
		unsigned int degree = 0 ;

		Traversor3WV<typename PFP::MAP> twv(map, d) ;
		for(Dart it = twv.begin(); it != twv.end(); it = twv.next())
		{
			degree++;
		}

		if (degree == 8)
		{
			//CAS HEXAEDRIQUE (ordre 2 quad superposes, le premier en CW)
			Dart e = d;
			Dart f = map.template phi<21121>(d);
			hexa.push_back(indices[f]);
			f = map.phi_1(f);
			hexa.push_back(indices[f]);
			f = map.phi_1(f);
			hexa.push_back(indices[f]);
			f = map.phi_1(f);
			hexa.push_back(indices[f]);
			hexa.push_back(indices[e]);
			e = map.phi1(e);
			hexa.push_back(indices[e]);
			e = map.phi1(e);
			hexa.push_back(indices[e]);
			e = map.phi1(e);
			hexa.push_back(indices[e]);
		}

		if (degree == 4)
		{
			//CAS TETRAEDRIQUE
			Dart e = d;
			tetra.push_back(indices[e]);
			e = map.phi1(e);
			tetra.push_back(indices[e]);
			e = map.phi1(e);
			tetra.push_back(indices[e]);
			e = map.template phi<211>(e);
			tetra.push_back(indices[e]);
		}
	}

	unsigned int nbhexa = hexa.size()/8;
	unsigned int nbtetra = tetra.size()/4;


	fout << "<?xml version=\"1.0\"?>" << std::endl;
	fout << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"BigEndian\">" << std::endl;
	fout << "  <UnstructuredGrid>" <<  std::endl;
	fout << "    <Piece NumberOfPoints=\"" << position.nbElements() << "\" NumberOfCells=\""<< (nbhexa+nbtetra) << "\">" << std::endl;
	fout << "      <Points>" << std::endl;
	fout << "        <DataArray type=\"Float32\" NumberOfComponents=\"3\" format=\"ascii\">" << std::endl;

	for (unsigned int i = position.begin(); i != position.end(); position.next(i))
	{
		const VEC3& P = position[i];
		fout << "          " << P[0]<< " " << P[1]<< " " << P[2] << std::endl;
	}

	fout << "        </DataArray>" << std::endl;
	fout << "      </Points>" << std::endl;
	fout << "      <Cells>" << std::endl;
	fout << "        <DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">" << std::endl;

	if (nbhexa!=0)
	{
		for (unsigned int i=0; i<nbhexa; ++i)
		{
			fout << "          ";
			fout << hexa[8*i]   << " " << hexa[8*i+1] << " " << hexa[8*i+2] << " " << hexa[8*i+3] << " " << hexa[8*i+4] << " ";
			fout << hexa[8*i+5] << " " << hexa[8*i+6] << " " << hexa[8*i+7] << std::endl;
		}
	}

	if (nbtetra != 0)
	{

		for (unsigned int i=0; i<nbtetra; ++i)
		{
			fout << "          ";
			fout << tetra[4*i] << " " << tetra[4*i+1] << " " << tetra[4*i+2] << " " << tetra[4*i+3] << std::endl;
		}
	}

	fout << "        </DataArray>" << std::endl;
	fout << "        <DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">" << std::endl;
	fout << "         ";
	unsigned int offset = 0;
	for (unsigned int i=0; i<nbhexa; ++i)
	{
		offset += 8;
		fout << " " << offset;
	}
	for (unsigned int i=0; i<nbtetra; ++i)
	{
		offset += 4;
		fout << " "<< offset;
	}

	fout << std::endl << "        </DataArray>" << std::endl;
	fout << "        <DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">" << std::endl;
	fout << "         ";
	for (unsigned int i=0; i<nbhexa; ++i)
	{
		fout << " 12";
	}
	for (unsigned int i=0; i<nbtetra; ++i)
	{
		fout << " 10";
	}

	fout << std::endl << "        </DataArray>" << std::endl;
	fout << "      </Cells>" << std::endl;
	fout << "    </Piece>" << std::endl;
	fout << "  </UnstructuredGrid>" << std::endl;
	fout << "</VTKFile>" << std::endl;

	fout.close();
	return true;
}



template <typename PFP>
bool exportMSH(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const char* filename)
{
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;

	// open file
	std::ofstream fout ;
	fout.open(filename, std::ios::out) ;

	if (!fout.good())
	{
		CGoGNerr << "Unable to open file " << filename << CGoGNendl ;
		return false ;
	}

	VertexAutoAttribute<unsigned int> indices(map,"indices_vert");

	fout << "$NOD" << std::endl;
	fout << position.nbElements()<< std::endl;
	unsigned int count=1;
	for (unsigned int i = position.begin(); i != position.end(); position.next(i))
	{
		const VEC3& P = position[i];
		fout << count << " " << P[0]<< " " << P[1]<< " " << P[2] << std::endl;
		indices[i] = count++;
	}


	std::vector<unsigned int> hexa;
	std::vector<unsigned int> tetra;
	hexa.reserve(2048);
	tetra.reserve(2048);

	TraversorW<MAP> trav(map) ;
	for(Dart d = trav.begin(); d != trav.end(); d = trav.next())
	{
		unsigned int degree = 0 ;

		Traversor3WV<typename PFP::MAP> twv(map, d) ;
		for(Dart it = twv.begin(); it != twv.end(); it = twv.next())
		{
			degree++;
		}

		if (degree == 8)
		{
			//CAS HEXAEDRIQUE (ordre 2 quad superposes, le premier en CW)
			Dart e = d;
			Dart f = map.template phi<21121>(d);
			hexa.push_back(indices[f]);
			f = map.phi_1(f);
			hexa.push_back(indices[f]);
			f = map.phi_1(f);
			hexa.push_back(indices[f]);
			f = map.phi_1(f);
			hexa.push_back(indices[f]);
			hexa.push_back(indices[e]);
			e = map.phi1(e);
			hexa.push_back(indices[e]);
			e = map.phi1(e);
			hexa.push_back(indices[e]);
			e = map.phi1(e);
			hexa.push_back(indices[e]);
		}

		if (degree == 4)
		{
			//CAS TETRAEDRIQUE
			Dart e = d;
			tetra.push_back(indices[e]);
			e = map.phi1(e);
			tetra.push_back(indices[e]);
			e = map.phi1(e);
			tetra.push_back(indices[e]);
			e = map.template phi<211>(e);
			tetra.push_back(indices[e]);
		}
	}

	fout << "$ENDNOD" << std::endl;
	fout << "$ELM" << std::endl;

	unsigned int countCell=1;
	unsigned int nbhexa = hexa.size()/8;
	unsigned int nbtetra = tetra.size()/4;

	fout << (nbhexa+nbtetra) << std::endl;
	if (nbhexa!=0)
	{
		for (unsigned int i=0; i<nbhexa; ++i)
		{
			fout << countCell++ << " 5 1 1 8 ";
			fout << hexa[8*i]   << " " << hexa[8*i+1] << " " << hexa[8*i+2] << " " << hexa[8*i+3] << " " << hexa[8*i+4] << " ";
			fout << hexa[8*i+5] << " " << hexa[8*i+6] << " " << hexa[8*i+7] << std::endl;
		}
	}

	if (nbtetra != 0)
	{

		for (unsigned int i=0; i<nbtetra; ++i)
		{
			fout << countCell++ << " 4 1 1 4 ";
			fout << tetra[4*i] << " " << tetra[4*i+1] << " " << tetra[4*i+2] << " " << tetra[4*i+3] << std::endl;
		}
	}

	fout << "$ENDELM" << std::endl;

	fout.close();
	return true;

}


template <typename PFP>
bool exportTet(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const char* filename)
{
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;

	// open file
	std::ofstream fout ;
	fout.open(filename, std::ios::out) ;

	if (!fout.good())
	{
		CGoGNerr << "Unable to open file " << filename << CGoGNendl ;
		return false ;
	}

	VertexAutoAttribute<unsigned int> indices(map,"indices_vert");

	fout << position.nbElements()<< " vertices" <<std::endl;

	std::vector<unsigned int> tetra;
	tetra.reserve(2048);


	unsigned int count=0;
	for (unsigned int i = position.begin(); i != position.end(); position.next(i))
	{
		indices[i] = count++;
	}

	TraversorW<MAP> trav(map) ;
	for(Dart d = trav.begin(); d != trav.end(); d = trav.next())
	{
		unsigned int degree = 0 ;

		Traversor3WV<typename PFP::MAP> twv(map, d) ;
		for(Dart it = twv.begin(); it != twv.end(); it = twv.next())
		{
			degree++;
		}

		if (degree == 4)
		{
			Dart e = d;
			tetra.push_back(indices[e]);
			e = map.phi_1(e);
			tetra.push_back(indices[e]);
			e = map.phi_1(e);
			tetra.push_back(indices[e]);
			e = map.template phi<211>(e);
			tetra.push_back(indices[e]);
		}
	}

	unsigned int nbtetra = tetra.size()/4;
	fout << nbtetra << " tets" << std::endl;

	for (unsigned int i = position.begin(); i != position.end(); position.next(i))
	{
		const VEC3& P = position[i];
		fout << P[0]<< " " << P[1]<< " " << P[2] << std::endl;
	}

	for (unsigned int i=0; i<nbtetra; ++i)
	{
		fout << "4 " << tetra[4*i] << " " << tetra[4*i+1] << " " << tetra[4*i+2] << " " << tetra[4*i+3] << std::endl;
	}

	fout.close();
	return true;
}


template <typename PFP>
bool exportNodeEle(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const char* filename)
{
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;

	std::string base(filename);
	size_t pos = base.rfind(".");
	base.erase(pos);
	std::string fnNode = base + ".node";
	std::string fnEle = base + ".ele";

	// open files
	std::ofstream fout ;
	fout.open(fnNode.c_str(), std::ios::out) ;


	if (!fout.good())
	{
		CGoGNerr << "Unable to open file " << filename << CGoGNendl ;
		return false ;
	}

	std::ofstream foutEle ;
	foutEle.open(fnEle.c_str(), std::ios::out) ;

	if (!foutEle.good())
	{
		CGoGNerr << "Unable to open file " << filename << CGoGNendl ;
		return false ;
	}

	VertexAutoAttribute<unsigned int> indices(map,"indices_vert");

	fout << position.nbElements()<< " 3  0  0"<<std::endl;
	unsigned int count=0;
	for (unsigned int i = position.begin(); i != position.end(); position.next(i))
	{
		const VEC3& P = position[i];
		fout << count << " " << P[0]<< " " << P[1]<< " " << P[2] << std::endl;
		indices[i] = count++;
	}

	fout.close();


	std::vector<unsigned int> hexa;
	std::vector<unsigned int> tetra;
	hexa.reserve(2048);
	tetra.reserve(2048);

	TraversorW<MAP> trav(map) ;
	for(Dart d = trav.begin(); d != trav.end(); d = trav.next())
	{
		unsigned int degree = 0 ;

		Traversor3WV<typename PFP::MAP> twv(map, d) ;
		for(Dart it = twv.begin(); it != twv.end(); it = twv.next())
		{
			degree++;
		}

		if (degree == 4)
		{
			//CAS TETRAEDRIQUE
			Dart e = d;
			tetra.push_back(indices[e]);
			e = map.phi1(e);
			tetra.push_back(indices[e]);
			e = map.phi1(e);
			tetra.push_back(indices[e]);
			e = map.template phi<211>(e);
			tetra.push_back(indices[e]);
		}
	}

	unsigned int countCell=0;
	unsigned int nbtetra = tetra.size()/4;

	foutEle << nbtetra << " 4  0" << std::endl;

	if (nbtetra != 0)
	{

		for (unsigned int i=0; i<nbtetra; ++i)
		{
			foutEle << countCell++ << "  " << tetra[4*i] << " " << tetra[4*i+1] << " " << tetra[4*i+2] << " " << tetra[4*i+3] << std::endl;
		}
	}

	foutEle.close();

	return true;
}





template <typename PFP>
bool exportVolBinGz(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const char* filename)
{
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;

	// open file
//	std::ofstream fout ;
//	fout.open(filename, std::ios::out) ;

	ogzstream fout(filename, std::ios::out|std::ios::binary);

	if (!fout.good())
	{
		CGoGNerr << "Unable to open file " << filename << CGoGNendl ;
		return false ;
	}

	VertexAutoAttribute<unsigned int> indices(map,"indices_vert");

	std::vector<typename PFP::VEC3> bufposi;
	bufposi.reserve(position.nbElements());

	unsigned int count=0;
	for (unsigned int i = position.begin(); i != position.end(); position.next(i))
	{
		const VEC3& P = position[i];
		bufposi.push_back(P);
//		fout << count << " " << P[0]<< " " << P[1]<< " " << P[2] << std::endl;
		indices[i] = count++;
	}
	if (count != bufposi.size())
		CGoGNerr << "Warning problem wrong nbElements in position attributes ?" << CGoGNendl;

	std::vector<unsigned int> hexa;
	std::vector<unsigned int> tetra;

	hexa.reserve(2048);
	tetra.reserve(2048);

	TraversorW<MAP> trav(map) ;
	for(Dart d = trav.begin(); d != trav.end(); d = trav.next())
	{
		unsigned int degree = 0 ;

		Traversor3WV<typename PFP::MAP> twv(map, d) ;
		for(Dart it = twv.begin(); it != twv.end(); it = twv.next())
		{
			degree++;
		}

		if (degree == 8)
		{
			//CAS HEXAEDRIQUE (ordre 2 quad superposes, le premier en CW)
			Dart e = d;
			hexa.push_back(indices[e]);
			e = map.phi1(e);
			hexa.push_back(indices[e]);
			e = map.phi1(e);
			hexa.push_back(indices[e]);
			e = map.phi1(e);
			hexa.push_back(indices[e]);

			e = map.template phi<2112>(e);
			hexa.push_back(indices[e]);
			e = map.phi1(e);
			hexa.push_back(indices[e]);
			e = map.phi1(e);
			hexa.push_back(indices[e]);
			e = map.phi1(e);
			hexa.push_back(indices[e]);

		}

		if (degree == 4)
		{
			//CAS TETRAEDRIQUE
			Dart e = d;
			tetra.push_back(indices[e]);
			e = map.phi1(e);
			tetra.push_back(indices[e]);
			e = map.phi1(e);
			tetra.push_back(indices[e]);
			e = map.template phi<211>(e);
			tetra.push_back(indices[e]);
		}
	}

	unsigned int nbhexa = hexa.size()/8;
	unsigned int nbtetra = tetra.size()/4;

	unsigned int buffer[3];
	buffer[0] = position.nbElements();
	buffer[1] = nbtetra;
	buffer[2] = nbhexa;

	fout.write((char*)(buffer),3*sizeof(unsigned int));

	// write positions
	fout.write((char*)(&(bufposi[0])),sizeof(VEC3)*bufposi.size());

	// write tetra indices if necessary
	if (nbtetra != 0)
	{

		long int nbo = (long int)&(tetra.back()) -(long int)&(tetra.front());
		nbo /= sizeof(unsigned int);
		if (nbo != (long int)(tetra.size()-1))
			CGoGNerr << "Memory vector problem"<< CGoGNendl;
		else
			fout.write((char*)(&(tetra[0])),sizeof(unsigned int)*tetra.size());
	}

	// write hexa indices if necessary
	if (nbhexa!=0)
	{
		long int nbo = (long int)&(hexa.back()) -(long int)&(hexa.front());
		nbo /= sizeof(unsigned int);
		if (nbo != (long int)(hexa.size()-1))
			CGoGNerr << "Memory vector problem"<< CGoGNendl;
		else
			fout.write((char*)(&(hexa[0])),sizeof(unsigned int)*hexa.size());
	}

	fout.close();
	return true;
}


template <typename PFP>
bool exportTetmesh(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const char* filename)
{
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;

	// open file
	std::ofstream fout ;
	fout.open(filename, std::ios::out) ;

	if (!fout.good())
	{
		CGoGNerr << "Unable to open file " << filename << CGoGNendl ;
		return false ;
	}

	VertexAutoAttribute<unsigned int> indices(map,"indices_vert");

	fout << "Vertices" << std::endl<< position.nbElements() << std::endl;

	std::vector<unsigned int> tetra;
	tetra.reserve(2048);

	unsigned int count=1;
	for (unsigned int i = position.begin(); i != position.end(); position.next(i))
	{
		const VEC3& P = position[i];
		fout << P[0]<< " " << P[1]<< " " << P[2] << " " << "0" << std::endl;
		indices[i] = count++;
	}


	TraversorW<MAP> trav(map) ;
	for(Dart d = trav.begin(); d != trav.end(); d = trav.next())
	{
#ifndef _OPTIMIZED_FOR_TETRA_ONLY_
		unsigned int degree = 0 ;
		Traversor3WV<typename PFP::MAP> twv(map, d) ;
		for(Dart it = twv.begin(); it != twv.end(); it = twv.next())
		{
			degree++;
		}

		if (degree == 4)
#endif
		{
			Dart e = d;
			tetra.push_back(indices[e]);
			e = map.phi_1(e);
			tetra.push_back(indices[e]);
			e = map.phi_1(e);
			tetra.push_back(indices[e]);
			e = map.template phi<211>(e);
			tetra.push_back(indices[e]);
		}
	}

	unsigned int nbtetra = tetra.size()/4;
	fout << "Tetrahedra" << std::endl << nbtetra << std::endl;

	for (unsigned int i=0; i<nbtetra; ++i)
	{
		fout << tetra[4*i] << " " << tetra[4*i+1] << " " << tetra[4*i+2] << " " << tetra[4*i+3] << std::endl;
	}

	fout.close();
	return true;
}


template <typename PFP>
bool exportMesh(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const std::string& filename)
{
	Import::ImportType kind = Import::getFileType(filename);

// manque TS, OFF ?
	switch (kind)
	{
	case Import::TET:
		return exportTet<PFP>(map, position, filename.c_str());
		break;
	case Import::NODE:
		return exportNodeEle<PFP>(map, position, filename.c_str());
		break;
	case Import::MSH:
		return exportMSH<PFP>(map, position, filename.c_str());
		break;
	case Import::VTU:
		return exportVTU<PFP>(map, position, filename.c_str());
		break;
	case Import::NAS:
		return exportNAS<PFP>(map, position, filename.c_str());
		break;
	case Import::VBGZ:
		return exportVolBinGz<PFP>(map, position, filename.c_str());
		break;
	case Import::TETMESH:
		return exportTetmesh<PFP>(map, position, filename.c_str());
		break;
	default:
		CGoGNerr << "unknown file format for " << filename << CGoGNendl;
		return false;
		break;
	}
}



} // namespace Export

}

} // namespace Algo

} // namespace CGoGN
