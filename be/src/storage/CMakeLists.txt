# This file is made available under Elastic License 2.0.
# This file is based on code available under the Apache license here:
#   https://github.com/apache/incubator-doris/blob/master/be/src/olap/CMakeLists.txt

# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

# where to put generated libraries
set(LIBRARY_OUTPUT_PATH "${BUILD_DIR}/src/storage")

# where to put generated binaries
set(EXECUTABLE_OUTPUT_PATH "${BUILD_DIR}/src/storage")

add_subdirectory(rowset)
add_subdirectory(fs)

add_library(Storage STATIC
    base_tablet.cpp
    comparison_predicate.cpp
    decimal12.cpp
    delete_handler.cpp
    del_vector.cpp
    hll.cpp
    in_list_predicate.cpp
    key_coder.cpp
    lru_cache.cpp
    memtable_flush_executor.cpp
    null_predicate.cpp
    kv_store.cpp
    olap_server.cpp
    options.cpp
    page_cache.cpp
    primary_index.cpp
    primary_key_encoder.cpp
    protobuf_file.cpp
    rowset_update_state.cpp
    update_compaction_state.cpp
    version_graph.cpp
    schema.cpp
    storage_engine.cpp
    data_dir.cpp
    short_key_index.cpp
    snapshot_manager.cpp
    snapshot_meta.cpp
    tablet.cpp
    tablet_manager.cpp
    tablet_meta.cpp
    tablet_meta_manager.cpp
    tablet_schema.cpp
    tablet_schema_map.cpp
    tablet_updates.cpp
    txn_manager.cpp
    types.cpp
    update_manager.cpp
    utils.cpp
    wrapper_field.cpp
    rowset/segment_v2/array_column_iterator.cpp
    rowset/segment_v2/binary_plain_page.cpp
    rowset/segment_v2/bitmap_index_reader.cpp
    rowset/segment_v2/bitmap_index_writer.cpp
    rowset/segment_v2/bitshuffle_page.cpp
    rowset/segment_v2/bitshuffle_wrapper.cpp
    rowset/segment_v2/column_iterator.cpp
    rowset/segment_v2/column_reader.cpp
    rowset/segment_v2/column_writer.cpp
    rowset/segment_v2/default_value_column_iterator.cpp
    rowset/segment_v2/dictcode_column_iterator.cpp
    rowset/segment_v2/encoding_info.cpp
    rowset/segment_v2/scalar_column_iterator.cpp
    rowset/segment_v2/index_page.cpp
    rowset/segment_v2/indexed_column_reader.cpp
    rowset/segment_v2/indexed_column_writer.cpp
    rowset/segment_v2/ordinal_page_index.cpp
    rowset/segment_v2/page_io.cpp
    rowset/segment_v2/binary_dict_page.cpp
    rowset/segment_v2/binary_prefix_page.cpp
    rowset/segment_v2/segment.cpp
    rowset/segment_v2/segment_writer.cpp
    rowset/segment_v2/storage_page_decoder.cpp
    rowset/segment_v2/block_split_bloom_filter.cpp
    rowset/segment_v2/bloom_filter_index_reader.cpp
    rowset/segment_v2/bloom_filter_index_writer.cpp
    rowset/segment_v2/bloom_filter.cpp
    rowset/segment_v2/parsed_page.cpp
    rowset/segment_v2/zone_map_index.cpp
    rowset/vectorized/rowset_writer_adapter.cpp
    rowset/vectorized/segment_chunk_iterator_adapter.cpp
    rowset/vectorized/segment_iterator.cpp
    rowset/vectorized/segment_options.cpp
    task/engine_batch_load_task.cpp
    task/engine_checksum_task.cpp
    task/engine_clone_task.cpp
    task/engine_storage_migration_task.cpp
    task/engine_publish_version_task.cpp
    task/engine_alter_tablet_task.cpp
    column_vector.cpp
    vectorized/aggregate_iterator.cpp
    vectorized/chunk_helper.cpp
    vectorized/column_eq_predicate.cpp
    vectorized/column_ge_predicate.cpp
    vectorized/column_gt_predicate.cpp
    vectorized/column_in_predicate.cpp
    vectorized/column_le_predicate.cpp
    vectorized/column_lt_predicate.cpp
    vectorized/column_ne_predicate.cpp
    vectorized/column_not_in_predicate.cpp
    vectorized/column_null_predicate.cpp
    vectorized/column_or_predicate.cpp
    vectorized/column_expr_predicate.cpp
    vectorized/conjunctive_predicates.cpp
    vectorized/convert_helper.cpp
    vectorized/delete_predicates.cpp
    vectorized/disjunctive_predicates.cpp
    vectorized/empty_iterator.cpp
    vectorized/merge_iterator.cpp
    vectorized/predicate_parser.cpp
    vectorized/projection_iterator.cpp
    vectorized/push_handler.cpp
    vectorized/row_source_mask.cpp
    vectorized/schema_change.cpp
    vectorized/tablet_reader.cpp
    vectorized/tablet_reader_params.cpp
    vectorized/meta_reader.cpp
    vectorized/seek_tuple.cpp
    vectorized/union_iterator.cpp
    vectorized/unique_iterator.cpp
    vectorized/column_aggregate_func.cpp
    vectorized/chunk_aggregator.cpp
    vectorized/delta_writer.cpp
    vectorized/memtable.cpp
    vectorized/base_compaction.cpp
    vectorized/cumulative_compaction.cpp
    vectorized/compaction.cpp
    vectorized/rowset_merger.cpp
    vectorized/column_predicate_rewriter.cpp
    vectorized/column_predicate_dict_conjuct.cpp
)
