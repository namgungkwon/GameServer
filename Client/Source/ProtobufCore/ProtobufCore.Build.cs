// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class ProtobufCore : ModuleRules
{
	public ProtobufCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		Type = ModuleType.External;

		PublicSystemIncludePaths.Add(Path.Combine(ModuleDirectory, "Include"));
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Include"));
		
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "libprotobuf.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "utf8_range.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "utf8_validity.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_base.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_borrowed_fixup_buffer.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_city.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_civil_time.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_cord.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_cord_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_cordz_functions.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_cordz_handle.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_cordz_info.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_cordz_sample_token.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_crc_cord_state.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_crc_cpu_detect.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_crc_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_crc32c.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_debugging_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_decode_rust_punycode.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_demangle_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_demangle_rust.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_die_if_null.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_examine_stack.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_exponential_biased.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_failure_signal_handler.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_flags_commandlineflag.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_flags_commandlineflag_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_flags_config.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_flags_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_flags_marshalling.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_flags_parse.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_flags_private_handle_accessor.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_flags_program_name.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_flags_reflection.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_flags_usage.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_flags_usage_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_generic_printer_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_graphcycles_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_hash.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_hashtable_profiler.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_hashtablez_sampler.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_int128.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_kernel_timeout_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_leak_check.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_log_entry.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_log_flags.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_log_globals.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_log_initialize.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_log_internal_check_op.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_log_internal_conditions.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_log_internal_fnmatch.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_log_internal_format.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_log_internal_globals.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_log_internal_log_sink_set.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_log_internal_message.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_log_internal_nullguard.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_log_internal_proto.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_log_internal_structured_proto.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_log_severity.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_log_sink.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_malloc_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_periodic_sampler.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_poison.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_profile_builder.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_random_distributions.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_random_internal_distribution_test_util.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_random_internal_entropy_pool.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_random_internal_platform.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_random_internal_randen.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_random_internal_randen_hwaes.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_random_internal_randen_hwaes_impl.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_random_internal_randen_slow.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_random_internal_seed_material.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_random_seed_gen_exception.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_random_seed_sequences.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_raw_hash_set.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_raw_logging_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_scoped_set_env.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_spinlock_wait.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_stacktrace.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_status.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_statusor.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_str_format_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_strerror.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_strings.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_strings_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_symbolize.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_synchronization.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_throw_delegate.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_time.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_time_zone.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_tracing_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_utf8_for_code_point.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "absl_vlog_config_internal.lib"));

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}