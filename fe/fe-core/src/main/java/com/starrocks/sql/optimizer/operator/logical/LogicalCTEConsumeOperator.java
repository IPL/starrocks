// This file is licensed under the Elastic License 2.0. Copyright 2021 StarRocks Limited.

package com.starrocks.sql.optimizer.operator.logical;

import com.starrocks.sql.optimizer.ExpressionContext;
import com.starrocks.sql.optimizer.OptExpression;
import com.starrocks.sql.optimizer.OptExpressionVisitor;
import com.starrocks.sql.optimizer.base.ColumnRefSet;
import com.starrocks.sql.optimizer.operator.OperatorType;
import com.starrocks.sql.optimizer.operator.OperatorVisitor;
import com.starrocks.sql.optimizer.operator.Projection;
import com.starrocks.sql.optimizer.operator.scalar.ColumnRefOperator;
import com.starrocks.sql.optimizer.operator.scalar.ScalarOperator;

import java.util.ArrayList;
import java.util.Map;
import java.util.Objects;

/*
 * This operator denotes the place in the query where a CTE is referenced. The number of CTEConsumer
 * nodes is the same as the number of references to CTEs in the query. The id in the CTEConsumer
 * operator corresponds to the CTEProducer to which it refers. There can be multiple CTEConsumers
 * referring to the same CTEProducer.
 * */
public class LogicalCTEConsumeOperator extends LogicalOperator {
    private final String cteId;

    private final Map<ColumnRefOperator, ColumnRefOperator> cteOutputColumnRefMap;

    public LogicalCTEConsumeOperator(String cteId, Map<ColumnRefOperator, ColumnRefOperator> cteOutputColumnRefMap) {
        super(OperatorType.LOGICAL_CTE_CONSUME, -1, null, null);
        this.cteId = cteId;
        this.cteOutputColumnRefMap = cteOutputColumnRefMap;
    }

    public LogicalCTEConsumeOperator(long limit, ScalarOperator predicate, Projection projection, String cteId,
                                     Map<ColumnRefOperator, ColumnRefOperator> cteOutputColumnRefMap) {
        super(OperatorType.LOGICAL_CTE_CONSUME, limit, predicate, projection);
        this.cteId = cteId;
        this.cteOutputColumnRefMap = cteOutputColumnRefMap;
    }

    public Map<ColumnRefOperator, ColumnRefOperator> getCteOutputColumnRefMap() {
        return cteOutputColumnRefMap;
    }

    @Override
    public ColumnRefSet getOutputColumns(ExpressionContext expressionContext) {
        if (projection != null) {
            return new ColumnRefSet(new ArrayList<>(projection.getColumnRefMap().keySet()));
        } else {
            return new ColumnRefSet(new ArrayList<>(cteOutputColumnRefMap.keySet()));
        }
    }

    public String getCteId() {
        return cteId;
    }

    @Override
    public <R, C> R accept(OperatorVisitor<R, C> visitor, C context) {
        return visitor.visitLogicalCTEConsume(this, context);
    }

    @Override
    public <R, C> R accept(OptExpressionVisitor<R, C> visitor, OptExpression optExpression, C context) {
        return visitor.visitLogicalCTEConsume(optExpression, context);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        if (!super.equals(o)) {
            return false;
        }
        LogicalCTEConsumeOperator that = (LogicalCTEConsumeOperator) o;
        return Objects.equals(cteId, that.cteId) &&
                Objects.equals(cteOutputColumnRefMap, that.cteOutputColumnRefMap);
    }

    @Override
    public int hashCode() {
        return Objects.hash(super.hashCode(), cteId, cteOutputColumnRefMap);
    }


    public static class Builder
            extends LogicalOperator.Builder<LogicalCTEConsumeOperator, LogicalCTEConsumeOperator.Builder> {
        private String cteId;

        private Map<ColumnRefOperator, ColumnRefOperator> cteOutputColumnRefMap;

        @Override
        public LogicalCTEConsumeOperator build() {
            return new LogicalCTEConsumeOperator(this);
        }

        @Override
        public LogicalCTEConsumeOperator.Builder withOperator(LogicalCTEConsumeOperator operator) {
            super.withOperator(operator);
            this.cteId = operator.cteId;
            this.cteOutputColumnRefMap = operator.cteOutputColumnRefMap;
            return this;
        }
    }

    private LogicalCTEConsumeOperator(LogicalCTEConsumeOperator.Builder builder) {
        super(OperatorType.LOGICAL_CTE_CONSUME, builder.getLimit(), builder.getPredicate(), builder.getProjection());
        this.cteId = builder.cteId;
        this.cteOutputColumnRefMap = builder.cteOutputColumnRefMap;
    }
}
