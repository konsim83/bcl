#include <bcl/bcl.hpp>
#include <bcl/containers/HashMap.hpp>
#include <limits>
#include <experimental/random>

int main(int argc, char** argv) {
  BCL::init();

  using value_type = std::size_t;

  size_t num_ops = 1000000;
  size_t key_space = std::numeric_limits<value_type>::max();

  // BCL::HashMap<value_type, value_type, BCL::djb2_hash<value_type>> map(2*num_ops*BCL::nprocs());
  BCL::HashMap<value_type, value_type> map(2*num_ops*BCL::nprocs());

  srand48(BCL::rank());
  std::experimental::reseed(BCL::rank());

  value_type keys[num_ops];

  for(size_t i=0; i< num_ops; i++) {
    keys[i] = std::experimental::randint<value_type>(0, std::numeric_limits<value_type>::max());
  }

  BCL::barrier();
  auto begin = std::chrono::high_resolution_clock::now();

  for (size_t i = 0; i < num_ops; i++) {
    value_type value = keys[i];

    // map[value] = value;
    // map.strictly_insert_atomic_impl_(value, value);
    map.insert_atomic_impl_(value, value);
  }

  BCL::barrier();
  auto end = std::chrono::high_resolution_clock::now();
  double duration = std::chrono::duration<double>(end - begin).count();

  BCL::print("Saw %lu collisions, that's %lf/insert\n", BCL::collisions_check,
             ((double)BCL::collisions_check) / num_ops);

  double duration_us = 1e6*duration;
  double latency_us = duration_us / num_ops;

  BCL::print("Latency is %lf us per AM. (Finished in %lf s)\n", latency_us, duration);

  BCL::finalize();
  return 0;
}
