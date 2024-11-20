#include <iomanip>
#include <iostream>
#include "seal/seal.h"

#include <x86intrin.h>

using namespace std;
using namespace seal;

// Benchmarking
size_t NUM_REPEATS = 100;
//std::chrono::time_point<std::chrono::high_resolution_clock> time_start;
//std::chrono::time_point<std::chrono::high_resolution_clock> time_end;
unsigned int aux;
unsigned long long time_start;
unsigned long long time_end;


// FHE params
size_t N = 8192;
string bench_name; // "tiny";  "small"; "medium"; 

// SEAL objects
vector<Ciphertext> ciphertexts;
vector<Plaintext> plaintexts; 
vector<Ciphertext> noise;
Evaluator* evaluator;
SEALContext* context;
EncryptionParameters* params;

void print_SEAL_options() {
	cout << "[APP] C++ version: \t" << __cplusplus << endl;
	cout << "[APP] SEAL_USE_INTRIN"
		<< "\t"
		<<
#ifdef SEAL_USE_INTRIN
		"ON"
#else
		"OFF"
#endif
		<< endl;
	cout << "[APP] SEAL_USE_INTEL_HEXL"
		<< "\t"
		<<
#ifdef SEAL_USE_INTEL_HEXL
		"ON"
#else
		"OFF"
#endif
		<< endl;
	cout << "[APP] SEAL_USE_MSGSL"
		<< "\t"
		<<
#ifdef SEAL_USE_MSGSL
		"ON"
#else
		"OFF"
#endif
		<< endl;
}

void print_time(string func_name, unsigned long long elapsed_time) {
	cout << "[CYCLES TOTAL] " << func_name << "\t" << setprecision(6) << right
	    << setw(10) << elapsed_time << " cycles" << endl;
	cout << "[TIME TOTAL] " << func_name << "\t" << setprecision(6) << right
	    << setw(10) << elapsed_time / float(3600) << " us" << endl;
	cout << "[CYCLES] " << func_name << "\t" << setprecision(6) << right
	    << setw(10) << elapsed_time / float(NUM_REPEATS) << " cycles" << endl;
	cout << "[TIME] " << func_name << "\t" << setprecision(6) << right
	    << setw(10) << elapsed_time / float(NUM_REPEATS) / float(3600) << " us" << endl;
}

void print_header(string header, int width = 50) {
	cout << endl;
	cout << setfill('=') << setw(width) << "=" << endl;
	int wl = width - header.size() - 2;
	cout << setw(wl) << " " << header << " " << setw(width - wl - 1) << '='
		<< endl;
	;
	cout << endl;
	cout << setfill(' ');
}

void init_evaluator(void) {
	ciphertexts = vector<Ciphertext>();

	// Set up SEAL parameters
	params = new EncryptionParameters(scheme_type::bgv);
	auto poly_modulus_degree = (size_t)pow(2, 13);
	params->set_poly_modulus_degree(poly_modulus_degree);
	
	if (bench_name == "tiny") {
		params->set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 43, 43, 60 }));
	}
	else if (bench_name == "small") {
		params->set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 59, 60, 60 }));
	}
	else if (bench_name == "medium") {
		params->set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 45, 46, 46, 46 }));
	}
	else {
		throw invalid_argument(bench_name);
	}
	params->set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, 30));


	context = new SEALContext(*params);

	evaluator = new Evaluator(*context);
	BatchEncoder encoder(*context);
	KeyGenerator keygen(*context);
	const SecretKey& secret_key = keygen.secret_key();
	PublicKey public_key;
	keygen.create_public_key(public_key);
	Encryptor encryptor(*context, public_key);

	vector<uint64_t> ins(poly_modulus_degree);
	for (int i = 0; i < poly_modulus_degree; i++) {
		ins[i] = i;
	}
	Plaintext plain;
	encoder.encode(ins, plain);
	size_t num_ctxts;
	if (bench_name == "tiny") {
		num_ctxts = 2;


	}
	else if (bench_name == "small") {
		num_ctxts = 1;

		plaintexts = vector<Plaintext>(2);

		//plaintexts[0] = new Plaintext();
		//plaintexts[1] = new Plaintext();
		encoder.encode(ins, plaintexts[0]);
		encoder.encode(ins, plaintexts[1]);

		noise = vector<Ciphertext>(128);
		for (int i = 0; i < noise.size(); i++) {
			encryptor.encrypt(plain, noise[i]);
			cout << ".";
		}
		cout << endl;
	}
	else if (bench_name == "medium") {
		num_ctxts = 1;

		plaintexts = vector<Plaintext>(1);

		encoder.encode(ins, plaintexts[0]);

		noise = vector<Ciphertext>(128);
		for (int i = 0; i < noise.size(); i++) {
			encryptor.encrypt(plain, noise[i]);
			evaluator->mod_switch_to_next_inplace(noise[i]);
			cout << ".";
		}
		cout << endl;
	} 
	cout << "Allocated plaintexts / noise" << endl;

	ciphertexts = vector<Ciphertext>(num_ctxts);
	for (int i = 0; i < num_ctxts; i++) {
		encryptor.encrypt(plain, ciphertexts[i]);
	}
	cout << "Allocated ciphertexts" << endl;

	print_SEAL_options();
}

void eval_tiny() {
	Ciphertext* res = new Ciphertext();
	try {
		evaluator->multiply(ciphertexts[0], ciphertexts[1], *res);
	}
	catch (exception& e) {
		cerr << "multiply threw exception: " << e.what() << endl << flush;
		throw e;
	}
	int output = 2;

	if (output >= ciphertexts.size()) {
		ciphertexts.resize(output + 1);
	}
	ciphertexts[output] = *res;
	delete res;
}

void eval_small() {
	Ciphertext* res = new Ciphertext();
	try {
		evaluator->multiply_plain(ciphertexts[0], plaintexts[0], *res);
		evaluator->add_plain_inplace(*res, plaintexts[1]);
		for (int i = 0; i < 128 / 2; i++) {
			evaluator->add_inplace(*res, noise[2 * i]);
		}
	}
	catch (exception& e) {
		cerr << "small threw exception: " << e.what() << endl << flush;
		throw e;
	}
	delete res;
}
 
void eval_medium() {
	Ciphertext* res = new Ciphertext();
	try {
		evaluator->sub_plain(ciphertexts[0], plaintexts[0], *res);
		evaluator->square_inplace(*res);
		evaluator->mod_switch_to_next_inplace(*res);

		for (int i = 0; i < 128 / 2; i++) {
			evaluator->add_inplace(*res, noise[2 * i]);
		}
	}
	catch (exception& e) {
		cerr << "medium threw exception: " << e.what() << endl << flush;
		throw e;
	}
	delete res;
}

void viand2023(string name) {
	bench_name = name;
	unsigned long long elapsed_time;

	// Init evaluator
	init_evaluator();
	cout << "init_evaluator done" << endl;

	// Ask enclave to compute workload
	elapsed_time = 0;
	
	//time_start = chrono::high_resolution_clock::now();
	time_start = __rdtscp(&aux);
	for (size_t i = 0; i < NUM_REPEATS; i++) {
		if (bench_name == "tiny") {
			eval_tiny();
		}
		else if (bench_name == "small") {
			eval_small();
		}
		else if (bench_name == "medium") {
			eval_medium();
		}
		else {
			throw invalid_argument(bench_name);
		}
	}
	//time_end = chrono::high_resolution_clock::now();
	time_end = __rdtscp(&aux);
	//elapsed_time += chrono::duration_cast<chrono::microseconds>(time_end - time_start).count();
	elapsed_time += time_end - time_start;

	print_time(bench_name, elapsed_time);
}
