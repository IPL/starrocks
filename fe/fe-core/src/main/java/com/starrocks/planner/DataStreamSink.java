// This file is made available under Elastic License 2.0.
// This file is based on code available under the Apache license here:
//   https://github.com/apache/incubator-doris/blob/master/fe/fe-core/src/main/java/org/apache/doris/planner/DataStreamSink.java

// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

package com.starrocks.planner;

import com.starrocks.thrift.TDataSink;
import com.starrocks.thrift.TDataSinkType;
import com.starrocks.thrift.TDataStreamSink;
import com.starrocks.thrift.TExplainLevel;

/**
 * Data sink that forwards data to an exchange node.
 */
public class DataStreamSink extends DataSink {
    private final PlanNodeId exchNodeId;

    private DataPartition outputPartition;

    private boolean isMerge;

    public DataStreamSink(PlanNodeId exchNodeId) {
        this.exchNodeId = exchNodeId;
    }

    @Override
    public PlanNodeId getExchNodeId() {
        return exchNodeId;
    }

    @Override
    public DataPartition getOutputPartition() {
        return outputPartition;
    }

    public void setPartition(DataPartition partition) {
        outputPartition = partition;
    }

    public void setMerge(boolean isMerge) {
        this.isMerge = isMerge;
    }

    @Override
    public String getExplainString(String prefix, TExplainLevel explainLevel) {
        StringBuilder strBuilder = new StringBuilder();
        strBuilder.append(prefix + "STREAM DATA SINK\n");
        strBuilder.append(prefix + "  EXCHANGE ID: " + exchNodeId + "\n");
        if (outputPartition != null) {
            strBuilder.append(prefix + "  " + outputPartition.getExplainString(explainLevel));
        }
        return strBuilder.toString();
    }

    @Override
    public String getVerboseExplain(String prefix) {
        StringBuilder strBuilder = new StringBuilder();
        if (outputPartition != null) {
            strBuilder.append(prefix).append("OutPut Partition: ").
                    append(outputPartition.getExplainString(TExplainLevel.VERBOSE));
        }
        strBuilder.append(prefix).append("OutPut Exchange Id: ").append(exchNodeId).append("\n");
        return strBuilder.toString();
    }

    @Override
    protected TDataSink toThrift() {
        TDataSink result = new TDataSink(TDataSinkType.DATA_STREAM_SINK);
        TDataStreamSink tStreamSink =
                new TDataStreamSink(exchNodeId.asInt(), outputPartition.toThrift());
        tStreamSink.setIs_merge(isMerge);
        result.setStream_sink(tStreamSink);
        return result;
    }

    @Override
    public boolean canUsePipeLine() {
        return true;
    }
}
