// @flow

import React from 'react';
import { connect } from 'react-redux';
import idx from 'idx';
import type { BrowserHistory } from 'history';
import type { Match } from 'react-router-dom';

import { beginPollingPoll } from '../actions';
import type { Poll } from '../records/polls';
import type { State } from '../redux/types';
import MainLayout from './MainLayout';

const PollResultRow = ({
  value,
  votes,
  percent,
}: {
  value: string;
  votes: number;
  percent: number;
}) => (
  <React.Fragment>
    <div className='poll-result-label-row'>
      <span className='poll-result-label'>{value}</span>
      <span className='poll-result-count'>
        {votes} {votes === 1 ? 'vote' : 'votes'}
      </span>
    </div>
    <div className='poll-result-chart-row'>
      <div className='poll-result-bar-container'>
        <div
          className='poll-result-bar'
          style={{'width': `${percent}%`}}
        />
      </div>
      <span className='poll-result-percent'>
        {Math.round(percent)}%
      </span>
    </div>
  </React.Fragment>
);

const PollResultChart = ({ poll: { subject, options } }: { poll: Poll }) => {
  const maxValue = Object.keys(options)
    .reduce((maxValue, key) => Math.max(maxValue, options[key]), 0);

  const optionRows = Object.keys(options)
    .map((key: string): [string, number] => [key, options[key]])
    .sort((a, b) => b[1] - a[1])
    .map(([value, votes], i) => (
      <PollResultRow
        key={i}
        value={value}
        votes={votes}
        percent={!maxValue ? 0 : (votes / maxValue) * 100}
      />
    ));

  return (
    <div>
      <h1 className='poll-title'>{subject}</h1>
      {optionRows}
    </div>
  );
};

type PollResultOwnProps = {|
  +match: Match
|};
type PollResultProps = {|
  ...PollResultOwnProps,
  +beginPollingPoll: () => void;
  +beginPollingPoll: (string) => () => void;
  +history: BrowserHistory;
  +id: string;
  +poll?: Poll;
|};

class PollResult extends React.Component<PollResultProps> {
  stopPolling: () => void;

  componentDidMount() {
    this.stopPolling = this.props.beginPollingPoll(this.props.id);
  }

  componentWillUnmount() {
    this.stopPolling();
  }

  render() {
    const { poll, history } = this.props;

    const pollChart = poll && !poll.loading
      ? <PollResultChart poll={poll} />
      : <h4 className='poll-loading'>Loading...</h4>;

    return (
      <MainLayout history={history}>
        <div className='container'>
          {pollChart}
        </div>
      </MainLayout>
    );
  }
}

function mapStateToProps(
  state: State,
  ownProps: PollResultOwnProps
): $Shape<PollResultProps> {
  const id = ownProps.match.params.poll;
  if (!id) {
    return {};
  }

  const poll = idx(state, _ => _.polls[id]);
  if (!poll) {
    return {};
  }

  return { id, poll };
}

function mapDispatchToProps(dispatch) {
  return {
    beginPollingPoll: function(id) {
      return dispatch(beginPollingPoll(id));
    },
  };
}

export default connect<PollResultProps, PollResultOwnProps, _, _, _, _>(
  mapStateToProps,
  mapDispatchToProps,
)(PollResult);
